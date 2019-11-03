// Fragment shader
#version 330 core

layout(location = 0) out vec4 pixColor;

in vec2 fragUV;

uniform sampler2D mainTex;

uniform float time;

void main()
{
  vec3 texColor = texture(mainTex, fragUV).rrr;
  pixColor = vec4(texColor, 1.0f);
}
