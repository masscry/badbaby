// Fragment shader
#version 330 core

layout(location = 0) out vec4 outColor;

in vec2 fragUV;
in vec2 projView;

uniform sampler2D mainTex;

const vec3 black = vec3(0.1f, 0.1f, 0.1f);
const vec3 water = vec3(0.0, 0.2f, 0.1f);
const vec3 land = vec3(0.3f, 0.3f, 0.1f);

uniform float border;

float edgeDetector(float z)
{
  z *= 192.0f;
  float d = fract(z);
  d = mix(d, 1.0f-d, step(mod(z, 2.0f), 1.0f));
  return d/fwidth(z)*0.5f;
}

void main()
{
  float fdepth = texture(mainTex, fragUV).r;

  float edge = 1.0f - edgeDetector(fdepth);

  vec3 waterColor = mix(black, water, fdepth/border)*max(0.5f, edge);
  vec3 landColor = mix(black, land, fdepth/(1.0f-border))*max(0.5f, edge);

  vec3 coldepth = mix(waterColor, landColor, step(border, fdepth)); 

  float dst = (1.0f-smoothstep(0.90f, 0.95f, length(projView)));
  outColor = vec4(coldepth*dst, dst);
}
