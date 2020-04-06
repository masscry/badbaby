#include <screen_t.hpp>
#include <worker.hpp>
#include <context.hpp>

#include <space.hpp>

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

      this->radarCamera = bb::camera_t::Orthogonal(
        -1.0f, 1.0f, 1.0f, -1.0f
      );

      this->camera = bb::camera_t::Orthogonal(
        -10.0f, 10.0f, 10.0f, -10.0f
      );

      this->fb = bb::framebuffer_t(512, 512);

      this->box = bb::postOffice_t::Instance().New("arenaScreen");

      this->spaceActorID = bb::workerPool_t::Instance().Register<sub3000::space_t>();

      bb::context_t::Instance().RegisterActorCallback(
        this->spaceActorID,
        bb::cmfKeyboard
      );

      if (!sub3000::RequestGenerateMap(this->spaceActorID))
      {
        assert(0);
        bb::Error("%s", "Map Gen Failed!");
        sub3000::PostExit();
      }

      this->debugMapShader = bb::shader_t::LoadProgramFromFiles(
        "hmap-debug.vp.glsl",
        "hmap-debug.fp.glsl"
      );

      this->debugMapMesh = bb::GeneratePlane(
        bb::vec2_t(512.0f, 256.0f),
        bb::vec3_t(0.0f),
        bb::vec2_t(0.0f),
        true
      );

    }

    void screen_t::UpdateUnits(const bb::linePoints_t& units)
    {
      this->units = bb::GenerateMesh(
        bb::DefinePoints(0.1f, units)
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

        this->camera.View() = glm::translate(
          glm::rotate(
            glm::mat4(1.0f),
            -state->Angle(),
            glm::vec3(0.0f, 0.0f, 1.0f)
          ),
          glm::vec3(-state->Pos(), 0.0f)
        );

      }

      if (auto mapReady = bb::As<bb::ext::done_t>(msg))
      {
        this->debugMapTex = bb::texture_t(
          mapReady->HeightMap().Width(), 
          mapReady->HeightMap().Height(),
          mapReady->HeightMap().Data()
        );
        this->debugMapTex.SetFilter(GL_LINEAR, GL_LINEAR);
      }
    }

    void screen_t::OnRender()
    {
      camera.Update();
      radarCamera.Update();

      bb::framebuffer_t::Bind(this->fb);

      glDisable(GL_CULL_FACE);
      glDisable(GL_DEPTH_TEST);
      glBlendFunc(
        GL_ONE, GL_ONE_MINUS_SRC_ALPHA
      );

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      bb::shader_t::Bind(this->debugMapShader);
      bb::texture_t::Bind(this->debugMapTex);
      this->debugMapShader.SetBlock(
        this->debugMapShader.UniformBlockIndex("camera"),
        this->camera.UniformBlock()
      );
      this->debugMapMesh.Render();

      bb::shader_t::Bind(this->shader);
      this->shader.SetBlock(
        this->shader.UniformBlockIndex("camera"),
        this->camera.UniformBlock()
      );

      if (this->units.Good())
      {
        this->units.Render();
      }

      this->shader.SetBlock(
        this->shader.UniformBlockIndex("camera"),
        this->radarCamera.UniformBlock()
      );
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

}