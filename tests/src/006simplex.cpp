#include <random>
#include <memory>
#include <cmath>

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <common.hpp>
#include <context.hpp>
#include <simplex.hpp>
#include <shapes.hpp>
#include <camera.hpp>

int main(int argc, char* argv[])
{
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<long> dist;

  if (bb::ProcessStartupArguments(argc, argv) != 0)
  {
    return -1;
  }

  auto& context = bb::context_t::Instance();
  auto simplex = bb::simplex_t(dist(mt));

  std::unique_ptr<float[]> simplexMap(new float[2048*1024]);

  float radius = 10.0f;

  for (int j = 0; j < 1024; ++j)
  {
    double z = j/10.24f;

    for (int i = 0; i < 2048; ++i)
    {
      double angle = (i/2048.0)*glm::two_pi<double>();
      simplexMap[j*2048 + i] = 1.0f - simplex(glm::dvec3(cos(angle)*radius, sin(angle)*radius, z));
    }
  }

  double maxVal = std::numeric_limits<double>::lowest();
  for (int j = 0; j < 1024; ++j)
  {
    for (int i = 0; i < 2048; ++i)
    {
      maxVal = (simplexMap[j*2048 + i] > maxVal)?(simplexMap[j*2048 + i]):(maxVal);
    }
  }

  for (int j = 0; j < 1024; ++j)
  {
    for (int i = 0; i < 2048; ++i)
    {
      simplexMap[j*2048 + i] /= maxVal;
    }
  }

  auto tex = bb::texture_t(2048, 1024, simplexMap.get());
  tex.GenerateMipmaps();
  tex.SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

  float zNearVal = 0.0f;
  float zFarVal = 20.0f;
  int stackDepth = 40;

  auto plane = bb::GeneratePlaneStack(glm::vec2(800.0f, 400.0f), stackDepth, zNearVal, zFarVal);

  auto renderProgram = bb::shader_t::LoadProgramFromFiles(
    "006simplex_vp.glsl",
    "006simplex_fp.glsl"
  );

  zNearVal = 0.1;
  zFarVal = 1000.0f;

  auto worldCamera = bb::camera_t::Perspective(45.0f, context.Width()/(float)context.Height(), zNearVal, zFarVal);
  auto bindCameraPoint = renderProgram.UniformBlockIndex("camera");

  worldCamera.View() = glm::lookAt(
    glm::vec3(0.0f, 0.0f, 20.0f),
    glm::vec3(0.0f, 0.0f, -1.0f),
    glm::vec3(0.0f, 1.0f, 0.0f)
  );

  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

  auto lastTick = glfwGetTime();

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  for(;;)
  {
    auto nowTick = glfwGetTime();
    auto delta = nowTick - lastTick;
    lastTick = nowTick;

    bb::framebuffer_t::Bind(context.Canvas());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    bb::shader_t::Bind(renderProgram);
    worldCamera.Update();
    renderProgram.SetBlock(bindCameraPoint, worldCamera.UniformBlock());

    bb::texture_t::Bind(tex);
    plane.Render();

    if (!context.Update())
    {
      break;
    }
    context.Title(std::to_string(1.0/delta));

    glm::vec3 offset(
      (context.IsKeyDown(GLFW_KEY_D)-context.IsKeyDown(GLFW_KEY_A))*delta*10,
      (context.IsKeyDown(GLFW_KEY_W)-context.IsKeyDown(GLFW_KEY_S))*delta*10,
      0.0f
    );

    worldCamera.View() = glm::translate(worldCamera.View(), offset);

  }

  return 0;
}