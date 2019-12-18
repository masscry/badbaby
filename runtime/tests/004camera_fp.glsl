// Fragment shader
#version 330 core

layout(location = 0) out vec4 pixColor;

in vec2 fragUV;

uniform sampler2D mainTex;

void main()
{
  vec4 texColor = texture(mainTex, fragUV);

  pixColor = vec4(texColor.rgb*gl_FragCoord.w/gl_FragCoord.z, texColor.a);
}
