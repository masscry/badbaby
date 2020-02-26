#include <context.hpp>
#include <arena.hpp>
#include <camera.hpp>
#include <shapes.hpp>
#include <worker.hpp>
#include <space.hpp>

#include <vector>

namespace sub3000
{

  namespace radar
  {

    void screen_t::OnPrepare()
    {
      bb::config_t menuConfig;
      menuConfig.Load("./arena.config");

      this->shader = bb::shader_t::LoadProgramFromFiles(
        menuConfig.Value("radar.shader.vp", "radar.vp.glsl").c_str(),
        menuConfig.Value("radar.shader.fp", "radar.fp.glsl").c_str()
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

      this->box = bb::postOffice_t::Instance().New("arenaScreen");

      this->spaceActorID = bb::workerPool_t::Instance().Register(
        std::unique_ptr<bb::role_t>(new sub3000::space_t())
      );

    }

    void screen_t::UpdateUnits(const bb::linePoints_t& units)
    {
      this->units = bb::GenerateMesh(
        bb::DefinePoints(0.02f, units)
      );
    }

    void screen_t::OnUpdate(double dt)
    {
      bb::workerPool_t::Instance().PostMessage(
        this->spaceActorID,
        bb::msg_t(new step_t(this->box->Address(), 1, dt))
      );

      auto msg = this->box->Wait();
      if (auto state = bb::As<state_t>(msg))
      {
        this->UpdateUnits(state->Units());
      }
    }

    void screen_t::OnRender()
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

      if (this->units.Good())
      {
        this->units.Render();
      }
      this->radar.Render();
    }

    void screen_t::OnCleanup()
    {
      bb::workerPool_t::Instance().Unregister(this->spaceActorID);
    }

    screen_t::screen_t()
    : scene_t(sceneID_t::radarScreen, "Radar")
    {
      ;
    }

  }

  glm::vec2 OffsetFromCenterOfScreen(glm::vec2 offset)
  {
    auto screenDim = bb::context_t::Instance().Dimensions();
    auto center = screenDim*0.5f;

    return center + screenDim*offset;
  }

  glm::vec2 SquareScreenPercent(float percent)
  {
    auto screenDim = bb::context_t::Instance().Dimensions();
    auto minDim = std::min(screenDim.x, screenDim.y);
    return glm::vec2(minDim*percent);
  }

  void arenaScene_t::OnPrepare()
  {
    bb::config_t menuConfig;
    menuConfig.Load("./arena.config");

    this->radarScreen.Prepare();
    this->radarPlane = bb::GeneratePlane(
      SquareScreenPercent(
        static_cast<float>(menuConfig.Value("arena.radar.size", 0.9))
      ),
      glm::vec3(
        OffsetFromCenterOfScreen(
          glm::vec2(
            static_cast<float>(menuConfig.Value("arena.radar.offset.x", 0.25)),
            static_cast<float>(menuConfig.Value("arena.radar.offset.y", 0.0))
          )
        ),
        0.0f
      )
    );

    this->camera = bb::camera_t::Orthogonal(
      0.0f,
      static_cast<float>(bb::context_t::Instance().Width()),
      static_cast<float>(bb::context_t::Instance().Height()),
      0.0f
    );

    this->shader = bb::shader_t::LoadProgramFromFiles(
      menuConfig.Value("arena.shader.vp", "desktop.vp.glsl").c_str(),
      menuConfig.Value("arena.shader.fp", "desktop.fp.glsl").c_str()
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
