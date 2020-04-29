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
      bb::config_t screenConfig;
      screenConfig.Load("./arena.config");

      this->shader = bb::shader_t::LoadProgramFromFiles(
        screenConfig.Value("radar.shader.vp", "radar.vp.glsl").c_str(),
        screenConfig.Value("radar.shader.fp", "radar.fp.glsl").c_str()
      );

      FILE* input = fopen(screenConfig.Value("radar.mesh", "radar.msh").c_str(), "rb");
      if (input == nullptr)
      { // resource not found!
        assert(0);
      }
      BB_DEFER(fclose(input));

      this->pointSize = static_cast<float>(screenConfig.Value("radar.pointSize", 0.2f));

      this->radar = bb::GenerateMesh(
        bb::meshDesc_t::Load(input)
      );

      this->radarCamera = bb::camera_t::Orthogonal(
        -1.0f, 1.0f, 1.0f, -1.0f
      );

      this->worldScale = static_cast<float>(screenConfig.Value("radar.scale", 10.0));

      this->camera = bb::camera_t::Orthogonal(
        -this->worldScale/4.0f,
         this->worldScale/4.0f,
         this->worldScale/4.0f,
        -this->worldScale/4.0f
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

    void screen_t::UpdateUnits(const bb::linePoints_t& units, float radarAngle)
    {
      this->units = bb::GenerateMesh(bb::DefinePoints(this->pointSize, units));

      this->radarLine = bb::GenerateLine(
        0.02f,
        bb::linePoints_t{
          {0.0f, 0.0f},
          bb::Dir(glm::radians(radarAngle))
        }
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
          this->UpdateUnits(state->Units(), state->RadarAngle());
        }

        float newScale = glm::mix(this->worldScale/4.0f, this->worldScale, glm::length(state->Vel()));

        this->camera = bb::camera_t::Orthogonal(
          -newScale, 
           newScale,
           newScale,
          -newScale
        );

        this->camera.View() = glm::translate(
          glm::rotate(
            glm::mat4(1.0f),
            -state->Angle(),
            glm::vec3(0.0f, 0.0f, 1.0f)
          ),
          glm::vec3(-state->Pos(), 0.0f)
        );
        this->depth = state->Depth();
      }

      if (auto mapReady = bb::As<bb::ext::hmDone_t>(msg))
      {
        this->debugMapTex = bb::texture_t(
          mapReady->HeightMap().Width(), 
          mapReady->HeightMap().Height(),
          mapReady->HeightMap().Data()
        );
        this->debugMapTex.SetFilter(GL_LINEAR, GL_LINEAR);
        this->depthSteps = mapReady->DistanceMap().Depth();
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
      this->debugMapShader.SetFloat(
        "border",
        this->depth/(this->depthSteps-1)
      );
      this->debugMapShader.SetFloat(
        "depthSteps",
        this->depthSteps
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
      this->radarLine.Render();
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