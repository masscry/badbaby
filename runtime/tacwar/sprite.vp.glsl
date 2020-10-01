// Vertex Shader
#version 330 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vUV;

uniform camera
{
  mat4 proj;
  mat4 view;
};

uniform mat4 model;

out vec2 fragUV;

void main()
{
  fragUV = vUV;
  gl_Position = proj * view * model * vec4(vPos.xyz, 1.0f);
}
