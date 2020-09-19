// Fragment shader
#version 330 core

layout(location = 0) out vec4 pixColor;

in vec2 fragUV;
in vec3 fragCol;

uniform sampler2D tileset;
uniform float minAlpha;

void main()
{
  vec4 fragm = texture(tileset, fragUV);
  vec3 color = fragm.rgb * fragCol;
  pixColor = vec4(color, max(minAlpha, fragm.a));
}