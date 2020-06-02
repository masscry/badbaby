#include <common.hpp>
#include <context.hpp>
#include <camera.hpp>
#include <shapes.hpp>
#include <image.hpp>

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

  auto splashImage = bb::LoadTGA("splash.tga");
  auto splashScreen = bb::texture_t(
    splashImage.Width(),
    splashImage.Height(),
    splashImage.Depth(),
    splashImage.Data()
  );

  float aspect = static_cast<float>(splashImage.Width()) / static_cast<float>(splashImage.Height());

  auto camera = bb::camera_t::Orthogonal(
    -1.0f,
     1.0f,
    -1.0f * aspect,
     1.0f * aspect
  );

  auto vao = bb::vao_t::CreateVertexAttribObject();
  {
    auto vboPos = bb::vbo_t::CreateArrayBuffer(vPos, sizeof(vPos), false);
    auto vboUV = bb::vbo_t::CreateArrayBuffer(vUV, sizeof(vUV), false);
    auto vboInd = bb::vbo_t::CreateElementArrayBuffer(vInd, sizeof(vInd), false);

    vao.BindVBO(vboPos, 0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    vao.BindVBO(vboUV, 1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    vao.BindIndecies(vboInd);
  }

  bb::meshDesc_t meshDesc;
  meshDesc.Buffers().emplace_back(
    bb::MakeVertexBuffer(vPos)
  );
  meshDesc.Buffers().emplace_back(
    bb::MakeVertexBuffer(vUV)
  );
  meshDesc.Indecies() = bb::MakeIndexBuffer(vInd);

  auto mesh = bb::GenerateMesh(meshDesc);

  auto lastTick = glfwGetTime();
  auto timePassed = 0.0f;

  enum class showMode_t
  {
    old = 0,
    mesh = 1
  } showMode = showMode_t::old;

  float deltaMult = 1.0f;

  for(;;)
  {
    glDisable(GL_CULL_FACE);

    auto nowTick = glfwGetTime();
    auto delta = static_cast<float>(nowTick - lastTick);
        
    timePassed += delta*deltaMult;
    lastTick = nowTick;

    camera.Update();

    bb::shader_t::Bind(splashProgram);
    splashProgram.SetFloat(splashTimeUniform, timePassed);
    splashProgram.SetBlock("camera", camera.UniformBlock());

    bb::framebuffer_t::Bind(context.Canvas());
    bb::texture_t::Bind(splashScreen);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    switch (showMode)
    {
    case showMode_t::old:
      bb::vao_t::Bind(vao);
      glEnableVertexAttribArray(0);
      glEnableVertexAttribArray(1);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
      glDisableVertexAttribArray(0);
      glDisableVertexAttribArray(1);
      break;
    case showMode_t::mesh:
      mesh.Render();
      break;
    default:
      // can't happen!
      assert(0);
      break;
    }

    if (timePassed >= 1.0f)
    {
      deltaMult = -1.0f;
    }
    if (timePassed <= 0.0f)
    {
      showMode = (showMode == showMode_t::old) ? showMode_t::mesh : showMode_t::old;
      deltaMult = 1.0f;
    }

    if (!context.Update())
    {
      break;
    }
  }



  return 0;
}