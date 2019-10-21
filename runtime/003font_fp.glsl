#version 330 core

layout(location = 0) out vec4 pixColor;

in vec2 fragUV;

uniform sampler2D mainTex;

const float glyph_center = 0.75;

void main()
{
  float distance = texture(mainTex, fragUV).a;
  float width = fwidth(distance);

  float glyph_alpha = smoothstep(glyph_center - width, glyph_center + width, distance);

  pixColor = vec4(glyph_alpha);
}
