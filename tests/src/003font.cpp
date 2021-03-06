#include <common.hpp>
#include <context.hpp>
#include <font.hpp>
#include <text.hpp>

int main(int argc, char* argv[])
{
  if (bb::ProcessStartupArguments(argc, argv) != 0)
  {
    return -1;
  }

  auto& context = bb::context_t::Instance();
  auto renderProgram = bb::shader_t::LoadProgramFromFiles("003font_vp.glsl", "003font_fp.glsl");

  bb::font_t font;
  font.Load("mono.config");

  auto text = bb::textStatic_t(font, "Проверка!", bb::vec2_t(1.0f/10.0f, 0.2f), 3);

  for(;;)
  {
    bb::shader_t::Bind(renderProgram);
    bb::framebuffer_t::Bind(context.Canvas());

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    text.Render();

    if (!context.Update())
    {
      break;
    }
  }

  return 0;
}