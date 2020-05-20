#include <context.hpp>
#include <arena.hpp>
#include <camera.hpp>
#include <shapes.hpp>
#include <worker.hpp>
#include <space.hpp>
#include <monfs.hpp>
#include <mapGen.hpp>

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
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

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
      ),
      bb::vec2_t(0.5f),
      false
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
      ),
      bb::vec2_t(0.5f),
      false
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
