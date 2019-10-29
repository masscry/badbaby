#include <common.hpp>
#include <context.hpp>
#include <camera.hpp>

glm::vec2 vPos[4] = {
  { -0.5f, -0.5f },
  { +0.5f, -0.5f },
  { +0.5f, +0.5f },
  { -0.5f, +0.5f}
};

glm::vec2 vUV[4] = {
  { 0.0f, 1.0f },
  { 1.0f, 1.0f },
  { 1.0f, 0.0f },
  { 0.0f, 0.0f }
};

uint16_t vInd[6] = {
  0, 1, 2, 0, 2, 3
};

int main(int argc, char* argv[])
{
  if (bb::ProcessStartupArguments(argc, argv) != 0)
  {
    return -1;
  }

  auto& context = bb::context_t::Instance();
  auto splashProgram = bb::shader_t::LoadProgramFromFiles(
    "005splash_vp.glsl",
    "005splash_fp.glsl"
  );
  auto splashTimeUniform = splashProgram.UniformLocation("time");

  auto splashScreen = bb::texture_t::LoadTGA("splash.tga");

  auto vboPos = bb::vbo_t::CreateArrayBuffer(vPos, sizeof(vPos), false);
  auto vboUV  = bb::vbo_t::CreateArrayBuffer(vUV,  sizeof(vUV),  false);
  auto vboInd = bb::vbo_t::CreateElementArrayBuffer(vInd, sizeof(vInd), false);
  auto vao    = bb::vao_t::CreateVertexAttribObject();

  vao.BindVBO(vboPos, 0, 2, GL_FLOAT, GL_FALSE, 0, 0);
  vao.BindVBO(vboUV,  1, 2, GL_FLOAT, GL_FALSE, 0, 0);
  vao.BindIndecies(vboInd);

  auto lastTick = glfwGetTime();
  auto timePassed = 0.0;

  for(;;)
  {
    auto nowTick = glfwGetTime();
    auto delta = nowTick - lastTick;
    timePassed += delta/20.0f;
    lastTick = nowTick;

    bb::shader_t::Bind(splashProgram);
    splashProgram.SetFloat(splashTimeUniform, timePassed);

    bb::framebuffer_t::Bind(context.Canvas());
    bb::vao_t::Bind(vao);
    bb::texture_t::Bind(splashScreen);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    if ((!context.Update()) || (timePassed > 1.0f))
    {
      break;
    }
  }

  return 0;
}