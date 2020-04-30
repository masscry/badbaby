// Fragment shader
#version 330 core

layout(location = 0) out vec4 outColor;

in vec2 fragUV;
in vec2 projView;

uniform sampler2D mainTex;

const vec3 black = vec3(0.0);
const vec3 water = vec3(0.0, 0.1f, 0.0);
const vec3 land = vec3(0.1f, 0.5f, 0.1f);

uniform float depthSteps;
uniform float border;

void main()
{
  float fdepth = round(texture(mainTex, fragUV).r*depthSteps)/depthSteps;

  vec3 waterColor = mix(black, water, fdepth/border);
  vec3 landColor = mix(black, land, fdepth/(1.0f-border));

  float stepVal = 1.0 - step(border, fdepth);
  vec3 coldepth = waterColor*stepVal + landColor*(1.0 - stepVal); 

  outColor = vec4(
    coldepth,
    1.0f-smoothstep(0.90f, 1.05f, length(projView))
  );
}