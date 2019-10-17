#version 330 core

layout(location = 0) out vec4 pixColor;

in vec4 fragCol;

void main()
{
  pixColor = fragCol;
}
