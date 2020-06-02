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

uniform mat4 model;

out vec3 fragCol;
out vec3 fragNormal;

void main()
{
  fragCol = vCol;
  fragNormal = vec3(view * vec4(vNorm, 0.0f));
  gl_Position = proj * view * model * vec4(vPos, 1.0f);
}
