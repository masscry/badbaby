// Fragment shader
#version 330 core

layout(location = 0) out vec4 pixColor;

in vec2 fragPos;
in float fragTime;

uniform vec4 lineColor;

void main()
{
  float pct = 1.0f - 2.0f*distance(fragPos,vec2(0.5));
  pixColor = mix(vec4(0.0f), lineColor, pct*(5.0f-fragTime)/5.0f);
}
