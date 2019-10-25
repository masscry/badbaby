#include <common.hpp>
#include <context.hpp>
#include <camera.hpp>
#include <font.hpp>

#include <glm/gtc/matrix_transform.hpp>

int main(int argc, char* argv[])
{
  if (bb::ProcessStartupArguments(argc, argv) != 0)
  {
    return -1;
  }

  auto& context = bb::context_t::Instance();
  auto renderProgram = bb::shader_t::LoadProgramFromFiles("004camera_vp.glsl", "004camera_fp.glsl");
  auto camera = bb::camera_t::Perspective(45.0f, context.Width()/(float)context.Height(), 0.1f, 100.0f);

  glm::vec3 pos = glm::vec3(0.0f, 0.0f, -3.0f);

  glm::vec3 dir = glm::vec3(0.0f, 0.0f, 1.0f);
  glm::vec3 up  = glm::vec3(0.0f, 1.0f, 0.0f);

  bb::font_t font;
  font.Load("mono.config");
  auto text = bb::text_t(font, "Проверка!", bb::vec2_t(1.0f/4.5f, 1.0f));

  auto bindPoint = renderProgram.UniformBlockIndex("camera");

  auto lastTick = glfwGetTime();

  for(;;)
  {
    auto nowTick = glfwGetTime();
    auto delta = nowTick - lastTick;

    bb::shader_t::Bind(renderProgram);
    bb::framebuffer_t::Bind(context.Canvas());

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    camera.Update();
    renderProgram.SetBlock(bindPoint, camera.UniformBlock());
    text.Render();

    if (!context.Update())
    {
      break;
    }

    float dist = (context.IsKeyDown(GLFW_KEY_W) - context.IsKeyDown(GLFW_KEY_S))*delta;
    pos += dir*dist;

    float side = (context.IsKeyDown(GLFW_KEY_D) - context.IsKeyDown(GLFW_KEY_A))*delta;
    pos += glm::cross(dir, up)*side;

    camera.View() = glm::translate(glm::mat4(1.0f), pos);

    lastTick = nowTick;
  }

  return 0;
}