// Fragment shader
#version 330 core

layout(location = 0) out vec4 pixColor;

in vec2 fragUV;

uniform sampler2D mainTex;

const float glyphCenter = 0.75;

uniform vec3 glyphColor;

void main()
{
  float distance = texture(mainTex, fragUV).a;
  float width = fwidth(distance);

  float glyphAlpha = smoothstep(glyphCenter - width, glyphCenter + width, distance);

  pixColor = vec4(glyphColor, glyphAlpha);
}
