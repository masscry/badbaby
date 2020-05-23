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

      if (auto worldMapFile = bb::ext::binstore_t::Read("world.bbw"))
      {
        auto worldMap = bb::ext::distanceMap_t(worldMapFile);
        if (worldMap.IsGood())
        {
          bb::workerPool_t::Instance().PostMessage(
            this->spaceActorID,
            bb::Issue<bb::ext::hmDone_t>(
              -1, 
              bb::ext::heightMap_t(worldMap.HeightMap()),
              std::move(worldMap)
            )
          );
        }
      }
      else
      {
        if (!sub3000::RequestGenerateMap(this->spaceActorID))
        {
          assert(0);
          bb::Error("%s", "Map Gen Failed!");
          sub3000::PostExit();
        }
      }

    }

    template<typename data_t>
    std::vector<data_t> Linearize(const std::deque<data_t>& items)
    {
      std::vector<data_t> result;
      result.reserve(items.size());

      for(auto item: items)
      {
        result.emplace_back(item);
        result.emplace_back(item);
        result.emplace_back(item);
        result.emplace_back(item);
      }
      return result;
    }

    template<typename data_t>
    std::vector<data_t> FillBuffer(size_t size, data_t value)
    {
      std::vector<data_t> result;
      result.resize(size, value);
      return result;
    }

    void screen_t::UpdateUnits(bb::linePoints_t&& units, float radarAngle, double dt)
    {
      auto defLine = bb::DefineLine(
        glm::vec3(0.0f), 0.02f, 
        bb::linePoints_t{
          {0.0f, 0.0f},
          bb::Dir(glm::radians(radarAngle))
        }
      );
      defLine.Buffers().emplace_back(
        bb::MakeVertexBuffer(FillBuffer(8, 4.0f))
      );
      this->radarLine = bb::GenerateMesh(
        defLine
      );

      for (auto& item: this->unitLife)
      {
        item += dt;
      }

      auto cursorLife = this->unitLife.begin();
      auto cursorUnit = this->unitPoints.begin();
      for (size_t index = 0, lastIndex = this->unitLife.size(); index < lastIndex; ++index)
      {
        if (*cursorLife > 5.0f)
        {
          cursorLife = this->unitLife.erase(cursorLife);
          cursorUnit = this->unitPoints.erase(cursorUnit);
          --lastIndex;
        }
        else
        {
          ++cursorLife;
          ++cursorUnit;
        }
      }

      auto camProjView = this->camera.Projection() * this->camera.View();

      for (auto item: units)
      {
        auto tmpItem = glm::vec4(item, 0.0f, 1.0f);

        tmpItem = camProjView * tmpItem;

        this->unitPoints.emplace_back(glm::vec2(tmpItem.x, -tmpItem.y));
        this->unitLife.emplace_back(0.0f);
      }

      if (!this->unitPoints.empty())
      {
        auto mesh = bb::DefinePoints(this->pointSize * 0.1f, this->unitPoints);
        mesh.Buffers().emplace_back(
          bb::MakeVertexBuffer(Linearize(this->unitLife))
        );
        this->units = bb::GenerateMesh(mesh);
      }
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
        this->UpdateUnits(
          std::move(state->Units()),
          state->RadarAngle(),
          dt
        );

        float newScale = this->worldScale/4.0f;
        
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

      bb::shader_t::Bind(this->shader);
      this->shader.SetBlock(
        this->shader.UniformBlockIndex("camera"),
        this->camera.UniformBlock()
      );

      this->shader.SetBlock(
        this->shader.UniformBlockIndex("camera"),
        this->radarCamera.UniformBlock()
      );

      this->radar.Render();
      if (this->units.Good())
      {
        this->units.Render();
      }
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