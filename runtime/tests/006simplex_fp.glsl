// Fragment shader
#version 330 core

layout(location = 0) out vec4 pixColor;

in vec2 fragUV;
in float depth;

uniform sampler2D mainTex;
uniform float time;

void main()
{
  float fdepth = texture(mainTex,
    vec2(fragUV.x + time/60.0f, fragUV.y)
  ).r;
  pixColor = vec4(
    mix(vec3(0.6, 0.6, 0.0), vec3(0.0, 0.0, 0.3), 1.0 - fdepth),
    1.0f
  );
}
