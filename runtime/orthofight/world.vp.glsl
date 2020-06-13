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
out vec3 fragNormal;

void main()
{
  fragCol = vCol;
  fragNormal = vec3(view * vec4(vNorm, 0.0f));
  gl_Position = proj * view * vec4(vPos.x*0.5f, vPos.y*0.5f, vPos.z*0.5f + vCol.r/5.0f, 1.0f);
}
