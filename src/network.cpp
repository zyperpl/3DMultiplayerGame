#include "network.hpp"

#include "log.hpp"
#include "config.hpp"

#include "player_tank.hpp"
#include "dynamic_entity.hpp"
#include "game.hpp"
#include "math.hpp"

Network::Network(const char *serverName, int port, const uint64_t &tick)
  : tick(tick)
{
  Log::i("Connecting to %s on port %d...\n", serverName, port);

  auto setSocketTimeout = [](int s, double ms) {
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = static_cast<long>(ms*1000);
    if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
      Log::e("Cannot set socket %s timeout!\n", s);
    }
  };
  
  // create new UDP datagram socket
  sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (sock == -1) Log::e("UDP socket creation error!\n");

  // set timeout of the socket
  setSocketTimeout(sock, 0.01);

  // create gameserver socket address
  memset(&server, 0, sizeof(struct sockaddr_in));
  server.sin_family = AF_INET;
  server.sin_port = htons(port);

  if (inet_aton(serverName, &server.sin_addr) == 0) Log::e("inet_aton error!\n");
}

Network::~Network()
{
  while (!packageQueue.empty())
  { 
    Log::i("Deleting unsend package of type %d.\n", static_cast<int>(packageQueue.front().package.type));
    this->deleteFirstPackage();
  }
}

size_t Network::send(std::string str)
{
  return this->send((void*)(str.c_str()), str.size());
}

size_t Network::send(void *data, size_t size)
{
  /*
  // DEBUG PACKET PRINT
  if (size < 10) return 0;
  //printf("Sending %p data of %lu bytes.\n", data, size);
  for (size_t i = 0; i < size; i++) printf("%8x ", ((uint8_t*)data)[i]); printf("\n");

  const int LENS[] = { sizeof(NetworkPackageType), sizeof(uint64_t), sizeof(size_t), sizeof(float), sizeof(float), sizeof(float), sizeof(float), sizeof(float), sizeof(float), sizeof(double), sizeof(double), sizeof(double) };
  const size_t LENS_N = sizeof(LENS)/sizeof(int);
  const char *NAMES[] = { "type", "tick", "length", "pos.x", "pos.y", "pos.z", "rot.x", "rot.y", "rot.z", "vel.x", "vel.y", "vel.z" };

  for (size_t i = 0, a = 0; i < LENS_N; i++)
  {
    printf("%3lu %8s) ", i, NAMES[i]);
    for (int j = 0; j < LENS[i]; j++, a++)
    {
      printf("%8x ", ((uint8_t*)data)[a]);
    }
    printf("\n");
  }
  // */

  return sendto(sock, data, size, 0, (struct sockaddr*)&server, sizeof(server));
}

void Network::beginCommunication(Game *game)
{
  while (game->isRunning())
  {
    // send data
    packageQueueMutex.lock();
    while (!packageQueue.empty())
    {
      auto container = packageQueue.front();
      auto package = container.package;
      auto tick = game->getTick();
      if (package.tick >= tick-1)
      {
        char *data = new char[sizeof(NetworkPackage)+package.length];
        memcpy(data, &package, sizeof(NetworkPackage));
        memcpy(data+sizeof(NetworkPackage), container.data, package.length);

        this->send(data, sizeof(NetworkPackage) + package.length);
        delete[] data;
      }

      deleteFirstPackage();
    }
    packageQueueMutex.unlock();

    // receive data
    struct sockaddr_storage sender;
    socklen_t senderSize = sizeof(sender);
    bzero(&sender, sizeof(sender));

    char buffer[NETWORK_BUFFER_SIZE];
    ssize_t size = recvfrom(sock, buffer, NETWORK_BUFFER_SIZE, 0, (struct sockaddr*)&sender, &senderSize);

    if (size > 0)
    {
      //for(ssize_t i = 0; i < size; i++) { printf("%x ", buffer[i]); } puts(" ---");

      auto parsePlayersData = [this](char *data, size_t size) {
        size_t onePackageSize = sizeof(uint64_t) + sizeof(vec3<float>) + sizeof(vec3<float>) + sizeof(vec3<double>);
        size_t packagesNum = size/onePackageSize;
        
        for (size_t i = 0; i < packagesNum; i++)
        {
          char *d = &data[i*onePackageSize];
          //printf("Player %lu. of %zu.\n", i+1, packagesNum);
          uint64_t id;
          vec3<float> pos, rot;
          vec3<double> vel;
          extractVariable(&d, id);
          extractVariable(&d, pos);
          extractVariable(&d, rot);
          extractVariable(&d, vel);
          /*
          printf("Player %lu: " VEC3F_STR " " VEC3F_STR " " VEC3F_STR "\n",
              id, VEC3(pos), VEC3(rot), VEC3(vel) );
          // */

          auto pair = networkEntities.find(id);
          if (pair != networkEntities.end())
          {
            // entity already exists
            //Log::i("Updating network player %lu.\n", id);
            pair->second->setPosition(pos);
            pair->second->setRotation(rot);
            auto *player = dynamic_cast<PlayerTank*>(pair->second.get());
            player->setVelocity(vel);
          } else
          {
            // entity does not exist
            //Log::i("Adding network player %lu.\n", id);
            PlayerTank *player = new PlayerTank();
            player->setPosition(pos);
            player->setRotation(rot);
            player->setVelocity(vel);
            
            this->networkEntities.insert(std::make_pair(id, player));
          }
        }
      };
      NetworkPackage package;
      memcpy(&package, buffer, sizeof(package));
      this->lastReceivedTick = package.tick;
      switch (package.type)
      {
        case NetworkPackageType::PlayerInfo:
        case NetworkPackageType::NewPlayer:
          {
            parsePlayersData(buffer+sizeof(package), package.length);
          }
          break;
        case NetworkPackageType::PlayerShot:
            
          break;
      }
    }

    // wait
    std::this_thread::sleep_for(std::chrono::milliseconds(NETWORK_UPDATE_INTERVAL_MS));
  }
}

bool Network::addPacket(NetworkPackageType type, NetworkPacketData data)
{
  NetworkPackage package;
  package.type = type;
  package.tick = tick;

  NetworkPackageContainer container;

  
  // calculate packet length and copy data
  switch (type)
  {
    case NetworkPackageType::PlayerInfo:
      {
        auto v1 = data.position;
        auto v2 = data.rotation;
        auto v3 = data.velocity;

        package.length = sizeof(v1)+sizeof(v2)+sizeof(v3);
        container.data = new char[package.length];
        memcpy(container.data+0, &v1, sizeof(v1));
        memcpy(container.data+sizeof(v1), &v2, sizeof(v2));
        memcpy(container.data+sizeof(v1)+sizeof(v2), &v3, sizeof(v3));
      }
      break;
    case NetworkPackageType::NewPlayer:
    case NetworkPackageType::PlayerShot:
    default:
      Log::e("Unknown package type %d!\n", static_cast<int>(type));
  }

  container.package = package;

  std::lock_guard<std::mutex> mutexGuard(packageQueueMutex);
  packageQueue.push(container);
  
  return (!packageQueue.empty());
}
