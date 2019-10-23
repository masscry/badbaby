// Fragment shader
#version 330 core

layout(location = 0) out vec4 pixColor;

in vec3 fragCol;

void main()
{
  pixColor = vec4(fragCol, 1.0);
}
