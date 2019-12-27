// Vertex shader
#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 uv;

uniform camera
{
  mat4 proj;
  mat4 view;
};

uniform mat4 model;

out vec2 fragUV;

void main()
{
  fragUV      = uv;
  gl_Position = proj * view * model * vec4(pos, 1.0f);
}
