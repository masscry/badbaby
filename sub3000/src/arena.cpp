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

    this->camera = bb::camera_t::Orthogonal(
      -1.0f, 1.0f, 1.0f, -1.0f
    );

    glDisable(GL_DEPTH_TEST);
  }

  void arenaScene_t::OnUpdate(double)
  {

  }

  void arenaScene_t::OnRender()
  {
    bb::framebuffer_t::Bind(bb::context_t::Instance().Canvas());

    glBlendFunc(
      GL_ONE, GL_ONE_MINUS_SRC_ALPHA
    );

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    bb::shader_t::Bind(this->shader);
    camera.Update();

    this->shader.SetBlock(
      this->shader.UniformBlockIndex("camera"),
      this->camera.UniformBlock()
    );

    this->radar.Render();
  }

  void arenaScene_t::OnCleanup()
  {
    ;
  }

  arenaScene_t::arenaScene_t()
  : scene_t(sceneID_t::arena, "Arena")
  {
    ;
  }

  arenaScene_t::~arenaScene_t()
  {

  }







}
