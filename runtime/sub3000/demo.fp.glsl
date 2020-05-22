// Fragment shader
#version 330 core

layout(location = 0) out vec4 pixColor;

in vec2 fragPos;

const vec4 lineColor = vec4(0.1f, 1.3f, 0.1f, 1.0f);

void main()
{
//  pixColor = vec4(1.0f);
//  pixColor = vec4(fragPos, 0.0f, 1.0f);

  float pct = 1.0f - 2.0f*distance(fragPos,vec2(0.5));
  pixColor = mix(vec4(0.0f), lineColor, pct);
}
