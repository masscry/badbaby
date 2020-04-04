#include <blur.hpp>

namespace
{
  const char* blurVShader =
  R"raw(
    #version 330 core

    layout(location = 0) in vec2 pos;
    layout(location = 1) in vec2 uv;

    out vec2 fragUV;

    void main()
    {
      fragUV = uv;
      gl_Position = vec4(pos, 0.0f, 1.0f);
    }
  )raw";

  const char* blurFShader =
  R"raw(
    #version 330 core

    layout(location = 0) out vec4 pixColor;

    in vec2 fragUV;

    uniform sampler2D tex;
    uniform float resolution;
    uniform float radius;
    uniform vec2 dir;

    void main()
    {
      vec4 sum = vec4(0.0);

      vec2 tc = fragUV;

      float blur = radius/resolution;

      float hstep = dir.x;
      float vstep = dir.y;

      sum += texture(tex, vec2(tc.x - 4.0*blur*hstep, tc.y - 4.0*blur*vstep)) * 0.0162162162;
      sum += texture(tex, vec2(tc.x - 3.0*blur*hstep, tc.y - 3.0*blur*vstep)) * 0.0540540541;
      sum += texture(tex, vec2(tc.x - 2.0*blur*hstep, tc.y - 2.0*blur*vstep)) * 0.1216216216;
      sum += texture(tex, vec2(tc.x - 1.0*blur*hstep, tc.y - 1.0*blur*vstep)) * 0.1945945946;

      sum += texture(tex, vec2(tc.x, tc.y)) * 0.2270270270;

      sum += texture(tex, vec2(tc.x + 1.0*blur*hstep, tc.y + 4.0*blur*vstep)) * 0.1945945946;
      sum += texture(tex, vec2(tc.x + 2.0*blur*hstep, tc.y + 3.0*blur*vstep)) * 0.1216216216;
      sum += texture(tex, vec2(tc.x + 3.0*blur*hstep, tc.y + 2.0*blur*vstep)) * 0.0540540541;
      sum += texture(tex, vec2(tc.x + 4.0*blur*hstep, tc.y + 1.0*blur*vstep)) * 0.0162162162;

      pixColor = sum;
    }
  )raw";

} // namespace

namespace bb
{

  void blur_t::Render()
  {
    bb::shader_t::Bind(this->shader);
    this->shader.SetVector2f("dir", bb::vec2_t(1.0f, 0.0f));
    this->shader.SetFloat("radius", 1.0f);
    this->shader.SetFloat("resolution", static_cast<float>(fboSize));

    bb::framebuffer_t::Bind(this->temp);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    bb::texture_t::Bind(this->src->Texture());
    this->plane.Render();
    glFlush();

    bb::framebuffer_t::Bind(*this->dst);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    this->shader.SetVector2f("dir", bb::vec2_t(0.0f, 1.0f));
    bb::texture_t::Bind(this->temp.Texture());
    this->plane.Render();
  }

  blur_t::blur_t()
  : src(nullptr),
    dst(nullptr)
  {
    ;
  }

  blur_t::blur_t(bb::framebuffer_t* src, bb::framebuffer_t* dst, int fboSize)
  : plane(bb::GeneratePlane(bb::vec2_t(2.0f, 2.0f), bb::vec3_t(0.0f), bb::vec2_t(0.5f))),
    shader(blurVShader, blurFShader),
    temp(fboSize, fboSize),
    src(src),
    dst(dst),
    fboSize(fboSize)
  {
    assert((src != nullptr) && (dst != nullptr));
  }

  blur_t::~blur_t()
  {

  }

} // namespace bb
