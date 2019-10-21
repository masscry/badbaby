#version 330 core

layout(location = 0) out vec4 pixColor;

in vec2 fragUV;

uniform sampler2D mainTex;

void main()
{
#define SDF_FONT
#ifdef SDF_FONT
  float dist = texture(mainTex, fragUV).a;
  dist = clamp(dist - 0.5, 0.0, 1.0)*2.0;
  pixColor = vec4(dist, dist, dist, 1.0);
#else
  pixColor = texture(mainTex, fragUV);
#endif
}
