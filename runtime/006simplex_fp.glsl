// Fragment shader
#version 330 core

layout(location = 0) out vec4 pixColor;

in vec2 fragUV;

uniform sampler2D mainTex;

const vec4 marineBlue = vec4(0.52f, 0.65f, 0.75f, 1.0f);

void main()
{
  vec3 texColor = vec3(round(texture(mainTex, fragUV).r*20.0f)/20.0f);
  pixColor = vec4(texColor, 1.0f)*marineBlue;
}
