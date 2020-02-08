#include <context.hpp>
#include <arena.hpp>
#include <camera.hpp>
#include <shapes.hpp>

#include <vector>

namespace sub3000
{

  void arenaScene_t::OnPrepare()
  {
    bb::config_t menuConfig;
    menuConfig.Load("./arena.config");

    this->pContext = &bb::context_t::Instance();
    this->shader = bb::shader_t::LoadProgramFromFiles(
      menuConfig.Value("shader.vp", "arena.vp.glsl").c_str(),
      menuConfig.Value("shader.fp", "arena.fp.glsl").c_str()
    );

    this->radar = bb::GenerateCircle(
      static_cast<int>(menuConfig.Value("circle.sides", 16.0)),
      static_cast<float>(menuConfig.Value("circle.radius", 0.5)),
      static_cast<float>(menuConfig.Value("circle.width", 0.1))
    );
    this->radarFrame = bb::framebuffer_t(1024, 1024);
    this->blur = bb::blur_t(&this->radarFrame, &this->pContext->Canvas(), 1024);
  }

  void arenaScene_t::OnUpdate(double)
  {

  }
  
  void arenaScene_t::OnRender()
  {
    bb::framebuffer_t::Bind(this->radarFrame);
    bb::shader_t::Bind(this->shader);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    this->radar.Render();

    this->blur.Render();
  }

  void arenaScene_t::OnCleanup()
  {
    this->pContext = nullptr;
  }

  arenaScene_t::arenaScene_t()
  : scene_t(sceneID_t::arena, "Arena"),
    pContext(nullptr)
  {
    ;
  }

  arenaScene_t::~arenaScene_t()
  {

  }







}