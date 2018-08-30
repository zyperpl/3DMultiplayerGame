#version 330 core

in vec2 UV;

uniform sampler2D renderedTexture;
int steps = 1;

void main()
{
  vec4 color = vec4(1,1,1,1);

  float intensity = pow(abs(0.5-UV.x), 4);
  color.r = texture2D(renderedTexture, UV + vec2((0.5-UV.x)*1.0 * intensity, 0)).r;
  color.g = texture2D(renderedTexture, UV + vec2((0.5-UV.x)*1.1 * intensity, 0)).g;
  color.b = texture2D(renderedTexture, UV + vec2((0.5-UV.x)*0.9 * intensity, 0)).b;
  
  gl_FragColor = color;

  color = vec4(0,0,0,1);
  
  //*
  float strength = dot(texture2D(renderedTexture, UV).rgb, vec3(1.0, 1.0, 1.0));

  if (strength > 0.01)
  {
    for (int i = 0; i < steps; i++)
    {
      color.rgb += texture2D(renderedTexture, UV - vec2(0.0001*i,0)).rgb*strength/10;
      color.rgb += texture2D(renderedTexture, UV - vec2(0,0.0001*i)).rgb*strength/10;
    }

    gl_FragColor.rgb += color.rgb;
  }
  // */
}
