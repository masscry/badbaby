// Vertex Shader
#version 330 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vCol;
layout(location = 2) in vec3 vNorm;

uniform camera
{
  mat4 proj;
  mat4 view;
};

out vec3 fragCol;
out vec3 fragNorm;

void main()
{
  fragCol = vCol;
  fragNorm = vNorm;
  gl_Position = proj * view * vec4(vPos, 1.0f);
}
