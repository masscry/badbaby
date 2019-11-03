#include <common.hpp>
#include <context.hpp>
#include <simplex.hpp>
#include <shapes.hpp>
#include <random>
#include <memory>

int main(int argc, char* argv[])
{
  if (bb::ProcessStartupArguments(argc, argv) != 0)
  {
    return -1;
  }

  auto& context = bb::context_t::Instance();
  auto simplex = bb::simplex_t(0);

  std::unique_ptr<float[]> simplexMap(new float[1024*1024]);

  for (int j = 0; j < 1024; ++j)
  {
    for (int i = 0; i < 1024; ++i)
    {
      simplexMap[j*1024 + i] = static_cast<float>(simplex(i/100.0, j/100.0, 0.0));
    }
  }

  auto renderProgram = bb::shader_t::LoadProgramFromFiles(
    "006simplex_vp.glsl",
    "006simplex_fp.glsl"
  );
  auto tex = bb::texture_t(1024, 1024, simplexMap.get());
  tex.GenerateMipmaps();
  tex.SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
  auto plane = bb::plane_t(glm::vec2(2.0f, 2.0f));

  for(;;)
  {
    bb::shader_t::Bind(renderProgram);
    bb::framebuffer_t::Bind(context.Canvas());
    bb::texture_t::Bind(tex);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    plane.Render();

    if (!context.Update())
    {
      break;
    }
  }

  return 0;
}