// Fragment shader
#version 330 core

layout(location = 0) out vec4 pixColor;

in float color;

void main()
{
  pixColor = vec4(0.1f, 1.0f, 0.1f, 1.0f)*color;
}
