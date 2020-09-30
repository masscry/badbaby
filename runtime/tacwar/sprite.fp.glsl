// Fragment shader
#version 330 core

layout(location = 0) out vec4 pixColor;

in vec2 fragUV;

uniform sampler2D sprite;
uniform float contrast;
uniform float bright;
uniform float radSel;

vec3 fixBC(vec3 col)
{
  return ((col - 0.5f) * contrast + 0.5f) + bright;
}

void main()
{
  vec4 texCol = texture(sprite, fragUV);
  float dist = distance(fragUV,vec2(0.5f));

  float radius = clamp(0.0f, 1.0f, step(dist, 0.50) - step(dist, 0.45));
  pixColor = vec4(clamp(fixBC(texCol.rgb), 0.0f, 1.0f), texCol.a) + radius*radSel*vec4(0.0f, 0.5f, 0.0f, 0.5f);
}
