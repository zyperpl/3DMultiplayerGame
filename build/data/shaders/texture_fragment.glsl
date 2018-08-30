#version 330 core

layout(location = 0) out vec4 color;

in vec2 UV;
in vec3 Normal_cameraspace;
in vec3 vertexPosition_cameraspace;
in vec3 vertexPosition_worldspace;

vec4 shadowCoord;

uniform sampler2D textureSampler;
uniform sampler2D shadowMap;

uniform mat4 depthV;
uniform mat4 depthP;

#define LIGHTS_MAX 10
in struct Light
{
  vec3 position;  // worldspace
  vec3 direction;  // cameraspace
  vec3 color;     // color intensities
  float power;
} light[LIGHTS_MAX];

vec3 applyLight(vec4 color, Light light, float shadow)
{
  vec3 n = normalize(Normal_cameraspace);
  vec3 l = normalize(light.direction);

  float cosTheta = clamp(dot(n, l), 0,1);

  float dist = length(vertexPosition_worldspace - light.position);

  vec3 ambientLight = vec3(0.05, 0.05, 0.05) * light.color;

  vec3 result = ambientLight + (1.0 - shadow) * color.rgb * light.color.rgb * cosTheta * light.power / (dist * dist);

  return result;
}

float calculateShadow(vec4 fragPosLightSpace)
{
  /*if (fragPosLightSpace.x >= 1.0 || fragPosLightSpace.x < -1.0
  ||  fragPosLightSpace.y >= 1.0 || fragPosLightSpace.y < -1.0) return 0.0;*/
  vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
  
  if (projCoords.x > 1.0 || projCoords.x < -1.0) return 0.0;

  projCoords = projCoords * 0.5f + 0.5f;
  float closestDepth = texture(shadowMap, projCoords.xy).r;
  float currentDepth = projCoords.x;
  float bias = 0.003;

  float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

  return shadow;
}

void main()
{
  vec4 tex = texture2D(textureSampler, UV).rgba;
  color = vec4(0,0,0,1);

	if (tex.a < 0.2) discard;
  if (vertexPosition_cameraspace.z < 0
  &&  vertexPosition_cameraspace.z > -5) 
  {
    tex.a = abs(vertexPosition_cameraspace.z)/10.0F;
  }
  
  shadowCoord = depthP * depthV * vec4(vertexPosition_worldspace,1);
  float shadow = calculateShadow(shadowCoord)/3.0;

  //*
  for (int i = 0; i < LIGHTS_MAX; i++)
  {
	  color.rgb += applyLight(tex, light[i], shadow);
  }

  float c = clamp((-vertexPosition_cameraspace.z)/200.0,0,1);
  color.r += c*0.9;
  color.g += c*0.9;
  color.b += c*1.0;

  color.a = tex.a;
  // */
}

