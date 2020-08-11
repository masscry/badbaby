#include <starrg.hpp>

#include <common.hpp>
#include <mailbox.hpp>
#include <shader.hpp>
#include <camera.hpp>
#include <shapes.hpp>
#include <image.hpp>
#include <worker.hpp>
#include <font.hpp>
#include <text.hpp>

std::mt19937& RandomEngine()
{
  static std::random_device rd;
  static std::mt19937 gen(rd());
  return gen;
}

void starrg_t::Update(double dt)
{
  bb::msg_t msg;
  while (this->box->Poll(&msg))
  {
    if (auto tileDesc = bb::As<bb::msg::dataMsg_t<bb::meshDesc_t>>(msg))
    {
      this->mesh = bb::GenerateMesh(
        tileDesc->Data()
      );
      continue;
    }
    // Unknown message!
    assert(0);
  }

  this->text.Update("%5.1f", 1.0/dt);
}

void starrg_t::Render()
{
  bb::framebuffer_t::Bind(this->context.Canvas());

  glDisable(GL_CULL_FACE);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  this->camera.Update();
  
  bb::shader_t::Bind(this->mapShader);
  this->mapShader.SetBlock("camera", camera.UniformBlock());

  bb::texture_t::Bind(tileset);
  if (this->mesh.Good())
  {
    this->mesh.Render();
  }
  
  bb::shader_t::Bind(this->fontShader);
  this->fontShader.SetBlock("camera", camera.UniformBlock());
  this->text.Render();
}

starrg_t::starrg_t(bb::context_t& context)
: context(context),
  camera(bb::camera_t::Orthogonal(
    0.0f,
    static_cast<float>(context.Width()),
    static_cast<float>(context.Height()),
    0.0f)
  ),
  mapShader(bb::shader_t::LoadProgramFromFiles("starrg.vp.glsl", "starrg.fp.glsl")),
  fontShader(bb::shader_t::LoadProgramFromFiles("font.vp.glsl", "font.fp.glsl")),
  tileset(bb::texture_t::LoadConfig("tiles.config")),
  box(bb::postOffice_t::Instance().New("StarView"))
{
  auto len = static_cast<int>(context.Height()/tileSize.y);

  this->world = bb::workerPool_t::Instance().Register<world_t>(glm::ivec2(len));
  this->context.RegisterActorCallback(
    this->world,
    bb::context_t::keyboard
  );

  this->font.Load("font.config");
  this->text = bb::textDynamic_t(this->font, bb::vec2_t(8.0f, -16.0f));
}

starrg_t::~starrg_t()
{
  bb::workerPool_t::Instance().Unregister(this->world);
}
