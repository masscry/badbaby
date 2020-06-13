// Fragment shader
#version 330 core

layout(location = 0) out vec4 pixColor;

in vec3 fragCol;
in vec3 fragNormal;

void main()
{
  float light = max(dot(fragNormal, vec3(1.0f, 1.0f, 1.0f)), 0.0f)*gl_FragCoord.w/gl_FragCoord.z+0.2f;
  pixColor = vec4(fragCol*light, 1.0f);
}
