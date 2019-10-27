#include <common.hpp>
#include <context.hpp>
#include <camera.hpp>
#include <font.hpp>
#include <text.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

glm::mat4 DirectionRotation(glm::vec3 dir, glm::vec3 up)
{
  glm::mat4 result;
  glm::vec3 xAxis = glm::normalize(glm::cross(up, dir));
  glm::vec3 yAxis = glm::normalize(glm::cross(dir, xAxis));

  result[0].x = xAxis.x;
  result[0].y = yAxis.x;
  result[0].z = dir.x;
  result[0].w = 0.0f;

  result[1].x = xAxis.y;
  result[1].y = yAxis.y;
  result[1].z = dir.y;
  result[1].w = 0.0f;

  result[2].x = xAxis.z;
  result[2].y = yAxis.z;
  result[2].z = dir.z;
  result[2].w = 0.0f;

  result[3].x = 0.0f;
  result[3].y = 0.0f;
  result[3].z = 0.0f;
  result[3].w = 1.0f;
  return result;
}

glm::vec3 quadPos[6] = {
  { -10.0f, -1.0f, -10.0f },
  {  10.0f, -1.0f, -10.0f },
  {  10.0f, -1.0f,  10.0f },
  { -10.0f, -1.0f, -10.0f },
  {  10.0f, -1.0f,  10.0f },
  { -10.0f, -1.0f,  10.0f }
};

glm::vec2 quadUV[6] = {
  { 0.0f, 0.0f },
  { 2.0f, 0.0f },
  { 2.0f, 2.0f },
  { 0.0f, 0.0f },
  { 2.0f, 2.0f },
  { 0.0f, 2.0f }
};

int main(int argc, char* argv[])
{
  if (bb::ProcessStartupArguments(argc, argv) != 0)
  {
    return -1;
  }

  auto& context = bb::context_t::Instance();
  auto renderProgram = bb::shader_t::LoadProgramFromFiles("004camera_vp.glsl", "004camera_fp.glsl");
  auto worldCamera = bb::camera_t::Perspective(45.0f, context.Width()/(float)context.Height(), 0.1f, 100.0f);
  auto hudCamera = bb::camera_t::Orthogonal(0.0f, context.Width(), 0.0f, context.Height());
  auto gridTexture = bb::texture_t::LoadConfig("grid.config");

  bb::font_t font;
  auto fontProgram = bb::shader_t::LoadProgramFromFiles("004camera_vp.glsl", "003font_fp.glsl");
  font.Load("mono.config");
  auto fpsText = bb::textDynamic_t(font, bb::vec2_t(16.0f, 32.0f));

  glm::vec3 pos = glm::vec3(0.0f, 0.0f, -3.0f);
  glm::vec3 dir = glm::vec3(0.0f, 0.0f, 1.0f);
  glm::vec3 up  = glm::vec3(0.0f, 1.0f, 0.0f);

  bb::vbo_t vPos = bb::vbo_t::CreateArrayBuffer(quadPos, sizeof(quadPos), false);
  bb::vbo_t vUV  = bb::vbo_t::CreateArrayBuffer(quadUV, sizeof(quadUV), false);
  bb::vao_t quad = bb::vao_t::CreateVertexAttribObject();

  quad.BindVBO(vPos, 0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  quad.BindVBO(vUV, 1, 2, GL_FLOAT, GL_FALSE, 0, 0);

  auto bindPoint    = renderProgram.UniformBlockIndex("camera");
  auto bindPointHUD = fontProgram.UniformBlockIndex("camera"); 

  auto lastTick = glfwGetTime();

  context.RelativeCursor(true);
  auto lastCursorPos = context.MousePos();

  for(;;)
  {
    auto nowTick = glfwGetTime();
    auto delta = nowTick - lastTick;

    bb::framebuffer_t::Bind(context.Canvas());

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    bb::shader_t::Bind(renderProgram);
    worldCamera.Update();
    renderProgram.SetBlock(bindPoint, worldCamera.UniformBlock());

    bb::vao_t::Bind(quad);
    bb::texture_t::Bind(gridTexture);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    bb::shader_t::Bind(fontProgram);
    hudCamera.Update();
    fontProgram.SetBlock(bindPointHUD, hudCamera.UniformBlock());

    char buffer[64];
    snprintf(buffer, sizeof(buffer), "FPS: %5.2f", 1.0/delta);

    fpsText.Update(buffer);
    fpsText.Render();

    if (!context.Update())
    {
      break;
    }

    auto newCursorPos = context.MousePos();
    lastCursorPos = newCursorPos;
    
    float dist = (context.IsKeyDown(GLFW_KEY_W) - context.IsKeyDown(GLFW_KEY_S))*delta;
    pos += dir*dist;

    float side = (context.IsKeyDown(GLFW_KEY_D) - context.IsKeyDown(GLFW_KEY_A))*delta;
    pos += glm::cross(dir, up)*side;

    worldCamera.View() = glm::translate(glm::mat4(1.0f), pos);
    lastTick = nowTick;
  }

  return 0;
}