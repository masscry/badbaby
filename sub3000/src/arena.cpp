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
    
    FILE* input = fopen(menuConfig.Value("radar.mesh", "radar.msh").c_str(), "rb");
    if (input == nullptr)
    { // resource not found!
      assert(0);
    }
    BB_DEFER(fclose(input));

    this->radar = bb::GenerateMesh(
      bb::meshDesc_t::Load(input)
    );      
  }

  void arenaScene_t::OnUpdate(double)
  {

  }
  
  void arenaScene_t::OnRender()
  {
    bb::framebuffer_t::Bind(this->pContext->Canvas());
    bb::shader_t::Bind(this->shader);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    this->radar.Render();
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
