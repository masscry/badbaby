// Vertex shader
#version 330 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vUV;
layout(location = 2) in vec4 vCol;

uniform camera
{
  mat4 proj;
  mat4 view;
};

uniform mat4 model;

out vec2 fragUV;
out vec4 fragCol;

void main()
{
  fragUV      = vUV;
  fragCol     = vCol;
  gl_Position = proj * view * model * vec4(vPos.xy - vec2(1.0, 0.5), 0.0f, 1.0f);
}
