// Vertex Shader
#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 uv;

uniform camera
{
  mat4 proj;
  mat4 view;
};

out vec2 fragUV;
out vec2 projView;

void main()
{
  vec2 movedUV = vec2(-uv.x*512.0f, -uv.y*256.0f);
  vec4 projUV = view * vec4(movedUV, 0.0f, 1.0f);

  gl_Position = proj * vec4(pos, 1.0f);
  projView = gl_Position.xy;

  fragUV = vec2(-projUV.x/512.0f, -projUV.y/256.0f);
}
