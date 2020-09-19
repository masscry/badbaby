// Vertex Shader
#version 330 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vUV;
layout(location = 2) in vec3 vCol;
layout(location = 3) in vec2 vShim;

uniform camera
{
  mat4 proj;
  mat4 view;
};

out vec2 fragUV;
out vec3 fragCol;

uniform float time;

void main()
{
  fragUV = vUV;
  fragCol = vCol;
  gl_Position = proj * view * vec4(vPos.xy + vShim*sin(time), vPos.z, 1.0f);
}
