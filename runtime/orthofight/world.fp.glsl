// Fragment shader
#version 330 core

layout(location = 0) out vec4 pixColor;

in vec3 fragCol;
in vec3 fragNorm;

void main()
{
  float shaded = max(dot(fragNorm, normalize(vec3(1.0f, 1.0f, 0.3f))), 0.0f);
  pixColor = vec4(fragCol*vec3(shaded) + 0.1f, 1.0f);
}
