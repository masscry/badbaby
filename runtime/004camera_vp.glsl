// Vertex Shader
#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 col;

uniform (std140) camera
{
  mat4 proj;
  mat4 view;
};

out vec3 fragCol;

void main()
{
  fragCol = col;
  gl_Position = proj * view * vec4(pos, 1.0f);
}
