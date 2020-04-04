// Fragment shader
#version 330 core

layout(location = 0) out vec4 outColor;

in vec2 fragUV;

uniform sampler2D mainTex;
uniform float time;

const vec3 black = vec3(0.0);
const vec3 water = vec3(0.0, 0.0, 0.3);
const vec3 land = vec3(0.6, 0.6, 0.0);

const float border = 0.4;

void main()
{
  float fdepth = texture(mainTex,
    vec2(fragUV.x + time/60.0f, fragUV.y)
  ).r;

  vec3 waterColor = mix(black, water, (1.0f - fdepth)/border);
  vec3 landColor = mix(black, land, (1.0f - fdepth)/(1.0f-border));

  float stepVal = step(border, fdepth);
  vec3 coldepth = waterColor*stepVal + landColor*(1.0 - stepVal); 

  outColor = vec4(
    coldepth,
    1.0f
  );
}
