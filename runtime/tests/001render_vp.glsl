// Vertex shader
#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec4 col;

out vec4 fragCol;

void main()
{
  fragCol     = col;
  gl_Position = vec4(pos, 1.0f);
}
