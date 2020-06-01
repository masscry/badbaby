// Vertex Shader
#version 330 core

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 uv;

uniform camera
{
  mat4 proj;
  mat4 view;
};

out vec2 fragUV;

void main()
{
  fragUV = uv;
  gl_Position = proj * view * vec4(pos, 0.0f, 1.0f);
}
