// Fragment shader
#version 330 core

layout(location = 0) out vec4 pixColor;

in vec2 fragUV;
in vec4 fragCol;

uniform sampler2D mainTex;

void main()
{
  pixColor = vec4(fragCol.rgb, texture(mainTex, fragUV).a);
}
