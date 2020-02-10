// Fragment shader
#version 330 core

layout(location = 0) out vec4 pixColor;

in float fragPos;

const vec4 lineColor = vec4(0.1f, 1.0f, 0.1f, 1.0f);

void main()
{
  float fragDist = 1.0-2.0*abs(fragPos - 0.5);
  pixColor = lineColor*fragDist;
}
