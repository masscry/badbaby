// Vertex Shader
#version 330 core

layout(location = 0) in vec2 pos;
layout(location = 1) in float dist;

out float color;

void main()
{
  color = dist;
  gl_Position = vec4(pos, 0.0f, 1.0f);
}
