#include <common.hpp>

#include <context.hpp>
#include <frameTimer.hpp>
#include <shader.hpp>
#include <shapes.hpp>
#include <camera.hpp>

int main(int argc, char* argv[])
{
  if (bb::ProcessStartupArguments(argc, argv) != 0)
  {
    return -1;
  }

  auto& context = bb::context_t::Instance();
  auto lineShader = bb::shader_t::LoadProgramFromFiles(
    "radar.vp.glsl", "radar.fp.glsl"
  );

  auto title = bb::GenerateMesh(
    bb::DefineNumber(glm::vec3(0.2f, 0.2f, 0.0f), 0.15f, glm::vec2(1.0f), "Close-Quarters Combat")
  );

  auto menu = bb::GenerateMesh(
    bb::DefineNumber(glm::vec3(5.0f, 10.0f, 0.0f), 0.15f, glm::vec2(1.0f), "Menu")
  );

  auto newGame = bb::GenerateMesh(
    bb::DefineNumber(glm::vec3(5.0f, 13.0f, 0.0f), 0.15f, glm::vec2(1.0f), "New Game")
  );

  auto loadGame = bb::GenerateMesh(
    bb::DefineNumber(glm::vec3(5.0f, 15.0f, 0.0f), 0.15f, glm::vec2(1.0f), "Load Game")
  );

  auto exitGame = bb::GenerateMesh(
    bb::DefineNumber(glm::vec3(5.0f, 17.0f, 0.0f), 0.15f, glm::vec2(1.0f), "Exit")
  );

  auto aspect = context.AspectRatio();

  auto camera = bb::camera_t::Orthogonal(
    0.0f, 20.0f*aspect, 20.0f, 0.0f
  );

  bb::frameTimer_t frameTimer;
  double now = 0.0;

  size_t totalLines = 0;
  size_t totalLinesDelta = 8;
  size_t menuLines = 0;
  size_t menuLinesDelta = 0;

  while(context.Update())
  {
    auto delta = frameTimer.Delta();

    now += delta;
    if (now > 0.02)
    {
      now = 0.0;
      totalLines+=totalLinesDelta;
      menuLines+=menuLinesDelta;
    }

    if (totalLines > title.TotalVertecies())
    {
      totalLines = title.TotalVertecies();
      totalLinesDelta = 0;
      menuLinesDelta = 8;
    }

    if (menuLines > loadGame.TotalVertecies()*2)
    {
      totalLines = 0;
      totalLinesDelta = 8;
      menuLines = 0;
      menuLinesDelta = 0;
    }

    bb::framebuffer_t::Bind(context.Canvas());
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    bb::shader_t::Bind(lineShader);

    lineShader.SetBlock(
      "camera",
      camera.UniformBlock()
    );

    lineShader.SetVector4f(
      "lineColor",
      glm::vec4(1.3f, 1.3f, 1.3f, 1.0f)
    );
    title.SpecialRender(totalLines);

    menu.SpecialRender(menuLines);


    lineShader.SetVector4f(
      "lineColor",
      glm::vec4(1.3f, 0.1f, 0.1f, 1.0f)
    );
    newGame.SpecialRender(menuLines);


    lineShader.SetVector4f(
      "lineColor",
      glm::vec4(0.3f, 0.3f, 0.3f, 1.0f)
    );
    loadGame.SpecialRender(menuLines);
    exitGame.SpecialRender(menuLines);
  }

  return 0;
}