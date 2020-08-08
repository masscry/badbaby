// Fragment shader
#version 330 core

layout(location = 0) out vec4 pixColor;

in vec2 fragUV;
in vec3 fragCol;

uniform sampler2D tileset;

void main()
{
  vec3 texColor = texture(tileset, fragUV).rgb * fragCol;
  pixColor = vec4(texColor, 1.0f);
}