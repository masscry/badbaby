// Fragment shader
#version 330 core

layout(location = 0) out vec4 pixColor;

in vec2 fragUV;
in float depth;

uniform sampler2D mainTex;

void main()
{
  float show = step(depth, texture(mainTex, fragUV).r);
  pixColor = vec4(vec3(show)*depth, show);
}
