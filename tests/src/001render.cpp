#include <common.hpp>
#include <context.hpp>

float vPos[3*3] = {
   0.0f,  1.0f, 0.0f,
  -1.0f, -1.0f, 0.0f,
   1.0f, -1.0f, 0.0f
};

float vCol[3*4] = {
  1.0f, 0.0f, 0.0f, 1.0f,
  0.0f, 1.0f, 0.0f, 1.0f,
  0.0f, 0.0f, 1.0f, 1.0f,
};

int main(int argc, char* argv[])
{
  if (bb::ProcessStartupArguments(argc, argv) != 0)
  {
    return -1;
  }

  auto& context = bb::context_t::Instance();
  auto renderProgram = bb::shader_t::LoadProgramFromFiles("001render_vp.glsl", "001render_fp.glsl");
  auto vao = bb::vao_t::CreateVertexAttribObject();

  vao.BindVBO(bb::vbo_t::CreateArrayBuffer(vPos, sizeof(vPos)), 0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  vao.BindVBO(bb::vbo_t::CreateArrayBuffer(vCol, sizeof(vCol)), 1, 4, GL_FLOAT, GL_FALSE, 0, 0);

  for(;;)
  {
    bb::shader_t::Bind(renderProgram);
    bb::framebuffer_t::Bind(context.Canvas());
    bb::vao_t::Bind(vao);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    if (!context.Update())
    {
      break;
    }
  }

  return 0;
}