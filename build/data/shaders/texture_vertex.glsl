#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;

out vec2 UV;
out vec3 Normal_cameraspace;
out vec3 vertexPosition_cameraspace;
out vec3 vertexPosition_worldspace;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

#define LIGHTS_MAX 10

uniform struct WorldLight
{
  vec3 position; // worldspace
  vec3 color;    // color intensities
  float power;
} world_light[LIGHTS_MAX];

out struct Light
{
  vec3 position; // worldspace
  vec3 direction;  // cameraspace
  vec3 color;     // color intensities
  float power;
} light[LIGHTS_MAX];

uniform vec2 textureWrap;
out vec4 shadowCoord;

void main()
{
  gl_Position = P * V * M * vec4(vertexPosition_modelspace,1);

  vertexPosition_worldspace = (M * vec4(vertexPosition_modelspace, 1)).xyz;
  vertexPosition_cameraspace = (V * vec4(vertexPosition_worldspace, 1)).xyz;

  if (vertexPosition_cameraspace.z < -5)
  {
    //gl_Position.y += (pow(-vertexPosition_cameraspace.z,2))/800.0;
  }

  for (int i = 0; i < LIGHTS_MAX; i++)
  {
    light[i].position = world_light[i].position;
    light[i].direction = (V * vec4(world_light[i].position,1)).xyz + -vertexPosition_cameraspace;
    light[i].color = world_light[i].color;
    light[i].power = world_light[i].power;
  }

  Normal_cameraspace = (V * M * vec4(vertexNormal_modelspace,0)).xyz;

  UV = vertexUV * textureWrap;
}
