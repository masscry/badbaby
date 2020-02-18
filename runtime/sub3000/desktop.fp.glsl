// Fragment shader
#version 330 core

layout(location = 0) out vec4 pixColor;

in vec2 fragUV;

uniform sampler2D mainTex;

void main()
{
  pixColor = texture(mainTex, fragUV);
}
