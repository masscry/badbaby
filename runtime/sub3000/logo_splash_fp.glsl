// Fragment shader
#version 330 core

layout(location = 0) out vec4 pixColor;

in vec2 fragUV;

uniform sampler2D mainTex;

uniform float time;

void main()
{
  vec3 texColor = clamp(mix(vec3(0.0f), texture(mainTex, fragUV).rgb, min(log(time), 1.0)), 0.0f, 1.0f);
  pixColor = vec4(texColor, 1.0f);
}
