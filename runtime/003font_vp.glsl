// Vertex shader
#version 330 core

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 uv;

out vec2 fragUV;

void main()
{
  fragUV      = uv;
  gl_Position = vec4(pos - vec2(1.0, 0.5), 0.0f, 1.0f);
}
