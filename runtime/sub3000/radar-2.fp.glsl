// Fragment shader
#version 330 core

layout(location = 0) out vec4 pixColor;

in vec2 fragPos;
in float fragTime;
in vec2 projView;

const vec4 lineColor = vec4(0.1f, 1.3f, 0.1f, 1.0f);

void main()
{
  float dst = (1.0f-smoothstep(0.90f, 0.95f, length(projView)));
  float pct = 1.0f - 2.0f*distance(fragPos,vec2(0.5));
  pixColor = mix(vec4(0.0f), lineColor, pct*(5.0f-fragTime)/5.0f);
  pixColor *= dst;
}
