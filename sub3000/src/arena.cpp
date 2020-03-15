#include <context.hpp>
#include <arena.hpp>
#include <camera.hpp>
#include <shapes.hpp>
#include <worker.hpp>
#include <space.hpp>
#include <monfs.hpp>

#include <vector>

namespace sub3000
{

  class monitorConfig_t: public bb::fs::processor_t
  {
  public:
    int OnChange(const char*, bb::fs::event_t)
    {
      sub3000::PostChangeScene(sub3000::sceneID_t::arena);
      return 0;
    }
  };

  namespace radar
  {

    void status_t::OnPrepare()
    {
      bb::config_t menuConfig;
      menuConfig.Load("./arena.config");

      this->shader = bb::shader_t::LoadProgramFromFiles(
        menuConfig.Value("status.shader.vp", "status.vp.glsl").c_str(),
        menuConfig.Value("status.shader.fp", "status.fp.glsl").c_str()
      );

      this->camera = bb::camera_t::Orthogonal(
        -12.0f, 500.0f, -256.0f, 256.0f
      );

      this->fb = bb::framebuffer_t(512, 512);
      this->box = bb::postOffice_t::Instance().New("arenaStatus");

      this->font = bb::font_t(menuConfig.Value("status.font", "mono.config"));
      this->text = bb::textDynamic_t(this->font, bb::vec2_t(12.0f, -24.0f));
    }

    void status_t::OnUpdate(double)
    {
      bb::msg_t msg;
      if (this->box->Poll(&msg))
      {
        if (auto status = bb::As<player::status_t>(msg))
        {
          this->text.Update(
            "POS:\t[%+6.3f;%+6.3f]\n"
            "ENGINE:\t%s\n"
            "OUTPUT:\t%+6.3f\n"
            "SPEED:\t[%+6.3f;%+6.3f]\n"
            "RUDDER:\t%s [%+6.3f]\n"
            "AVEL:\t%+6.3f\n"
            "ANGLE:\t%+6.3f\n"
            "DRAG:\t%+6.3f",
            status->Data().pos.x, status->Data().pos.y,
            engine::ToString(status->Data().engine),
            status->Data().engineOutput,
            status->Data().vel.x, status->Data().vel.y,
            rudder::ToString(status->Data().rudder), status->Data().rudderPos*180.0/M_PI,
            status->Data().aVel*180.0/M_PI,
            status->Data().angle*180.0/M_PI,
            status->Data().dragCoeff
          );
        }
      }
    }

    void status_t::OnRender()
    {
      bb::framebuffer_t::Bind(this->fb);
      
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      bb::shader_t::Bind(this->shader);
      camera.Update();

      this->shader.SetBlock(
        this->shader.UniformBlockIndex("camera"),
        this->camera.UniformBlock()
      );

      this->shader.SetVector3f(
        "glyphColor", glm::vec3(0.2f, 0.7f, 0.0f)
      );
      this->shader.SetMatrix(
        "model",
        glm::mat4(1.0f)
      );
      this->text.Render();
    }

    void status_t::OnCleanup()
    {
      this->box.reset();
    }

    bb::framebuffer_t& status_t::Framebuffer()
    {
      return this->fb;
    }

    const bb::framebuffer_t& status_t::Framebuffer() const
    {
      return this->fb;
    }

    status_t::status_t()
    : scene_t(sceneID_t::radarStatus, "Status")
    {
      ;
    }

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

      this->spaceActorID = bb::workerPool_t::Instance().Register<sub3000::space_t>();

      bb::context_t::Instance().RegisterActorCallback(
        this->spaceActorID,
        bb::cmfKeyboard
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
        bb::msg_t(new step_t(this->box->Address(), dt))
      );

      auto msg = this->box->Wait();
      if (auto state = bb::As<state_t>(msg))
      {
        if (!state->Units().empty())
        {
          this->UpdateUnits(state->Units());
        }
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
      this->box.reset();
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

    this->radarStatus.Prepare();
    this->radarStatusPlane = bb::GeneratePlane(
      SquareScreenPercent(
        static_cast<float>(menuConfig.Value("arena.status.size", 0.9))
      ),
      glm::vec3(
        OffsetFromCenterOfScreen(
          glm::vec2(
            static_cast<float>(menuConfig.Value("arena.status.offset.x", -0.25)),
            static_cast<float>(menuConfig.Value("arena.status.offset.y", 0.0))
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

    this->box = bb::postOffice_t::Instance().New("arenaBox");
    this->configWatch = -1;

    sub3000::PostToMain(
      bb::Issue<sub3000::fs::watch_t>(
        this->box->Address(),
        "./arena.config",
        std::unique_ptr<bb::fs::processor_t>(new monitorConfig_t)
      )
    );
  }

  void arenaScene_t::OnUpdate(double dt)
  {
    bb::msg_t msg;
    if (this->box->Poll(&msg))
    {
      if (auto status = bb::As<sub3000::fs::status_t>(msg))
      {
        this->configWatch = status->Status();
      }
    }

    this->radarScreen.Update(dt);
    this->radarStatus.Update(dt);
  }

  void arenaScene_t::OnRender()
  {
    this->radarScreen.Render();
    this->radarStatus.Render();

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

    bb::texture_t::Bind(this->radarStatus.Framebuffer().Texture());
    this->radarStatusPlane.Render();
  }

  void arenaScene_t::OnCleanup()
  {
    this->radarStatus.Cleanup();
    this->radarScreen.Cleanup();

    if (this->configWatch != -1)
    {
      sub3000::PostToMain(
        bb::Issue<sub3000::fs::rmWatch_t>(this->configWatch)
      );
    }
    this->box.reset();
  }

  arenaScene_t::arenaScene_t()
  : scene_t(sceneID_t::arena, "Arena"),
    configWatch(-1)
  {

  }

}
