#version 330 core

layout(location = 0) out float fragmentDepth;

uniform sampler2D textureSampler;
in vec2 UV;

void main()
{
  vec4 tex = texture2D(textureSampler, UV).rgba;
	if (tex.a < 0.2) discard;
  //fragmentDepth = gl_FragCoord.z;
}
