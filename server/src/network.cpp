#include "network.hpp"
#include "game.hpp"

Server::Server(int port)
{ 
  // creating socket
  sfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (sfd == -1)
  {
    fprintf(stderr, "Cannot create socket!\n");
  }
  
  struct sockaddr_in server;
  memset((char *) &server, 0, sizeof(server));

  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  server.sin_addr.s_addr = htonl(INADDR_ANY);

  if (fcntl(sfd, F_SETFL, O_NONBLOCK))
  {
    fprintf(stderr, "Cannot make socket non-blocking!\n");
    close(sfd);
  }
  /*int optionValue = 1;
  if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optionValue, sizeof(optionValue)))
  {
    fprintf(stderr, "Cannot set socket address reuse!\n");
    close(sfd);
  }*/

  if (bind(sfd, (struct sockaddr*)&server, sizeof(server) ) == -1)
  {
    fprintf(stderr, "Cannot bind sfdet to port!\n");
    close(sfd);
  }


  // creating epoll
  efd = epoll_create(1); 
  if (efd == -1)
  {
    fprintf(stderr, "Cannot create epoll!\n");
    close(sfd);
  }

  struct epoll_event event;
  event.events = EPOLLIN | EPOLLET;
  event.data.u64 = 0LL;
  event.data.fd = sfd;
  if ((epoll_ctl(efd, EPOLL_CTL_ADD, sfd, &event) == -1))
  {
    fprintf(stderr, "Cannot add server socket to epoll!\n");
    close(sfd);
    close(efd);
  }
}

Server::~Server()
{
  close(efd);
  close(sfd);
}

void Server::process(Game *game)
{
  //static long debugIterationN = 0; printf("Iteration %ld.\n", debugIterationN++);

  struct epoll_event events[EPOLL_SIZE];
  int num = epoll_wait(efd, events, EPOLL_SIZE, EPOLL_TIMEOUT);

  for (int i = 0; i < num; ++i)
  {
    // debug print
    //auto _e = events[i].events;printf("\r socket=%d, events(%d): %s;%s;%s\n", efd, _e, _e & EPOLLIN ? "EPOLLIN" : "", _e & EPOLLOUT ? "EPOLLOUT" : "", _e & EPOLLERR || _e & EPOLLHUP ? "EPOLLERR || EPOLLHUP" : "");

    // check for error
    if (events[i].events & EPOLLERR
    ||  events[i].events & EPOLLHUP)
    {
      fprintf(stderr, "EPOLL ERROR! events=%d", events[i].events);
      close(events[i].data.fd);
      continue;
    } 

    // receive event on server socket
    if (sfd == events[i].data.fd
    &&  events[i].events & EPOLLIN)
    {
      uint8_t buffer[BUFFER_SIZE];
      struct sockaddr inAddr;
      socklen_t inLen = sizeof(inAddr);
      int len = recvfrom(sfd, buffer, BUFFER_SIZE, 0, &inAddr, &inLen);
      buffer[len] = '\0';
      
      if (len > 0)
      {
        auto id = sockaddrID((struct sockaddr_in*)&inAddr);
        
        char host[64], port[64];
        getnameinfo(&inAddr, inLen, host, sizeof(host), port, sizeof(port), NI_NUMERICHOST | NI_NUMERICSERV);
        //printf(" Received from %s:%s (%d bytes). \n", host, port, len); 
        //for (int i = 0; i < len; i++) { printf("%8x ", buffer[i]); } printf("\n");
        
        if ((unsigned long)len < sizeof(NetworkPackage))
        {
          printf("Wrong package format (%d bytes -ne %lu bytes)!\n", len, sizeof(NetworkPackage));
          break;
        }
        NetworkPackage package;
        memcpy(&package, buffer, sizeof(NetworkPackage));

        //printf("type=%d tick=%lu length=%lu\n\n", package.type, package.tick, package.length);

        char *data = new char[package.length];
        memcpy(data, buffer+sizeof(NetworkPackage), package.length);
        
        NetworkPackageContainer container;
        container.package = package;
        container.data = data;

        /*
        const int LENS[] = { 
          sizeof(NetworkPackageType), sizeof(package.tick), sizeof(package.length), 
          sizeof(float), sizeof(float), sizeof(float), sizeof(float), sizeof(float), sizeof(float), 
          sizeof(double), sizeof(double), sizeof(double) };
        const size_t LENS_N = sizeof(LENS)/sizeof(int);
        const char *NAMES[] = { "type", "tick", "length", "pos.x", "pos.y", "pos.z", "rot.x", "rot.y", "rot.z", "vel.x", "vel.y", "vel.z" };

        for (size_t i = 0, a = 0; i < LENS_N; i++)
        {
          printf("%3lu %8s) ", i, NAMES[i]);
          for (int j = 0; j < LENS[i]; j++, a++)
          {
            printf("%8x ", buffer[a]);
          }
          printf("\n");
        }
        // */

        const auto extractPlayerData = [](NetworkPackageContainer c)->const struct PlayerData
        {
          assert(sizeof(PlayerData) == c.package.length);
          PlayerData p;
          memcpy(&p, c.data, c.package.length);
          return p;
        };

        switch (container.package.type)
        {
          case NetworkPackageType::PlayerInfo:
            {
              auto d = extractPlayerData(container);
              /*
              printf(VEC3F_STR ", " VEC3F_STR ", " VEC3F_STR ".\n", 
                  VEC3(d.position), VEC3(d.rotation), VEC3(d.velocity) );
              // */
              auto *player = game->getPlayer(id);
              if (player) 
              {
                player->setLastTick(game->getTick());
                if (!player->checkData(d.position, d.rotation, d.velocity))
                {
                  player->setData(d.position, d.rotation, d.velocity);
                  //printf("Data missmatch with player %d!\n", id);
                }
                // exit if player already exists
                break;
              }
            }
            // continue if player does not exist
          case NetworkPackageType::NewPlayer:
            {
              auto d = extractPlayerData(container);
              game->addPlayer(id, d, inAddr);
              break;
            }
          case NetworkPackageType::PlayerShot:
            break;
        }
        
      } //if (len > 0)
    } //if(sfd == events[i].data.fd && events[i].events & EPOLLIN)
  }
}

void Server::send(Game *game)
{
  auto players = game->getPlayers();
  
  for (auto &p : players)
  {
    auto *player = p.second;
    if (player->getLastTick() < game->getTick()-TICK_LIMIT)
    {
      // player timed out
      printf("Player %lu timed out!\n", p.first);
      game->removePlayer(p.first);
      printf("Players: %lu\n", game->getPlayers().size());
      continue;
    }

    this->send(player, game);
  }
}

void Server::send(Player *player, Game *game)
{  
  const int MIN_PLAYER_N = 1;

  auto players = game->getPlayers();
  int playersNum = players.size();

  if (playersNum <= MIN_PLAYER_N) return;

  struct __attribute__((packed)) playerPackageData
  {
    uint64_t id;
    vec3<float> position;
    vec3<float> rotation;
    vec3<double> velocity;
  };
  const auto createPlayerPackage = [](char **buffer, uint64_t id, Player *player) {
    playerPackageData pack;
    pack.id = id;
    pack.position = player->getPosition();
    pack.rotation = player->getRotation();
    pack.velocity = player->getVelocity();

    memcpy(*buffer, &pack, sizeof(pack));
    *buffer += sizeof(pack);
  };

  size_t bufferSize = (playersNum-MIN_PLAYER_N) * sizeof(playerPackageData);

  char buffer[bufferSize];
  char *b = &buffer[0];
  for (auto &p : players)
  {
    if (MIN_PLAYER_N >= 1) if (p.second == player) continue;
    createPlayerPackage(&b, p.first, p.second);
  }

  NetworkPackage package;
  package.type = NetworkPackageType::PlayerInfo;
  package.tick = game->getTick();
  package.length = bufferSize;

  char data[sizeof(package)+bufferSize];
  memcpy(data, &package, sizeof(package));
  memcpy(data+sizeof(package), buffer, bufferSize);

  //for(size_t i = 0; i < sizeof(data); i++) { printf("%x ", data[i]); } puts(" ---");

  auto addr = player->getSockAddr();
  sendto(sfd, &data[0], sizeof(data), 0, (struct sockaddr*)&addr, sizeof(addr));
}
