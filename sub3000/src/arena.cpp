#include <context.hpp>
#include <arena.hpp>
#include <camera.hpp>
#include <shapes.hpp>

#include <vector>

namespace sub3000
{

  void radarScreen_t::OnPrepare()
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

    this->fb = bb::framebuffer_t(512, 512);
  }

  void radarScreen_t::OnUpdate(double)
  {

  }

  void radarScreen_t::OnRender()
  {
    bb::framebuffer_t::Bind(this->fb);

    glDisable(GL_DEPTH_TEST);
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

  void radarScreen_t::OnCleanup()
  {
    ;
  }

  radarScreen_t::radarScreen_t()
  : scene_t(sceneID_t::radarScreen, "Radar")
  {
    ;
  }

  glm::vec3 CenterOfScreen()
  {
    return glm::vec3(
      bb::context_t::Instance().Dimensions()/2.0f,
      0.0f
    );
  }

  void arenaScene_t::OnPrepare()
  {
    this->radarScreen.Prepare();
    this->radarPlane = bb::GeneratePlane(
      glm::vec2(100.0f, 100.0f),
      CenterOfScreen()
    );

    this->camera = bb::camera_t::Orthogonal(
      0.0f,
      static_cast<float>(bb::context_t::Instance().Width()),
      static_cast<float>(bb::context_t::Instance().Height()),
      0.0f
    );

    this->shader = bb::shader_t::LoadProgramFromFiles(
      "desktop.vp.glsl",
      "desktop.fp.glsl"
    );

  }

  void arenaScene_t::OnUpdate(double dt)
  {
    this->radarScreen.Update(dt);
  }

  void arenaScene_t::OnRender()
  {
    this->radarScreen.Render();
    bb::framebuffer_t::Bind(bb::context_t::Instance().Canvas());

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    bb::shader_t::Bind(this->shader);
    camera.Update();

    this->shader.SetBlock(
      this->shader.UniformBlockIndex("camera"),
      this->camera.UniformBlock()
    );

    bb::texture_t::Bind(this->radarScreen.Framebuffer().Texture());
    this->radarPlane.Render();
  }

  void arenaScene_t::OnCleanup()
  {
    this->radarScreen.Cleanup();
  }

  arenaScene_t::arenaScene_t()
  : scene_t(sceneID_t::arena, "Arena")
  {

  }

}
