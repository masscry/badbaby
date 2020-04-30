// Fragment shader
#version 330 core

layout(location = 0) out vec4 pixColor;

in float fragPos;
in float fragTime;

const vec4 lineColor = vec4(0.1f, 1.3f, 0.1f, 1.0f);

void main()
{
  float fragDist = (1.0-2.0*abs(fragPos - 0.5))*0.3;
  pixColor = mix(vec4(0.0f), lineColor, fragDist*(10.0f-fragTime)/10.0f);
}