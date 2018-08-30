#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

out vec2 UV;

uniform vec2 textureWrap;

void main()
{
  gl_Position = P * V * M * vec4(vertexPosition_modelspace, 1); 
  UV = vertexUV * textureWrap;
}
