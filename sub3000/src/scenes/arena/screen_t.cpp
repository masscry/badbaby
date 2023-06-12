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

      this->warningText = screenConfig.Value("radar.warning", "warning").c_str();

      FILE* input = fopen(screenConfig.Value("radar.mesh", "radar.msh").c_str(), "rb");
      if (input == nullptr)
      { // resource not found!
        BB_PANIC();
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

      this->fb = bb::framebuffer_t(1024, 1024);

      this->box = bb::postOffice_t::Instance().New("arenaScreen");

      this->spaceActorID = bb::workerPool_t::Instance().Register<sub3000::space_t>();

      bb::context_t::Instance().RegisterActorCallback(
        this->spaceActorID,
        bb::context_t::keyboard
      );

      bool worldMapReady = false;

      if (auto worldMapFile = bb::ext::binstore_t::Read("world.bbw"))
      {
        try
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
            worldMapReady = true;
          }
        }
        catch (const std::runtime_error& error)
        {
          bb::Error(
            "Can't load worldMap: %s. REGENERATE!",
            error.what()
          );
        }
      }

      if (worldMapReady == false)
      {
        if (!sub3000::RequestGenerateMap(this->spaceActorID))
        {
          BB_PANIC();
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

    void screen_t::UpdateUnits(bb::linePoints_t&& units, float radarAngle, float dt)
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
        auto mesh = bb::DefinePoints(this->pointSize * 0.01f, this->unitPoints);
        mesh.Buffers().emplace_back(
          bb::MakeVertexBuffer(Linearize(this->unitLife))
        );
        this->units = bb::GenerateMesh(mesh);
      }
    }

    void screen_t::UpdateDepthRadar(const state_t& state)
    {
      auto points = bb::linePoints_t();
      float zpos = 0.0f;

      this->mayCollide = false;

      int totalRadarPoints = static_cast<int>(state.RadarZ().size());

      for (int i = -totalRadarPoints/2; i < totalRadarPoints/2; ++i)
      {
        auto zdepth = state.RadarZ()[static_cast<size_t>(i + totalRadarPoints/2)];

        points.emplace_back(
          glm::vec2(zpos - 0.5f, zdepth)
        );
        zpos += 0.05f;

        if ((state.Player().engine - engine::mode_t::stop) <= 0)
        {
          this->mayCollide = this->mayCollide
            || ((zdepth <= 0.0f) && i >= 0);
        }
        else
        {
          this->mayCollide = this->mayCollide
            || ((zdepth <= 0.0f) && i <= 0);
        }
      }

      auto tmap = 
        glm::scale(
          glm::translate(
            glm::mat3(1.0f),
            glm::vec2(0.775f, -0.8f)
          ),
          glm::vec2(0.35f, 0.5f)
        );

      for (auto& zpoint: points)
      {
        zpoint = tmap * glm::vec3(zpoint, 1.0f);
      }

      auto zpoints = bb::DefinePoints(
        this->pointSize * ((this->mayCollide)?(0.07f):(0.01f)),
        points
      );

      if (this->mayCollide)
      {
        zpoints.Append(
          bb::DefineNumber(
            glm::vec3(0.655f, -0.76f, 0.0f),
            this->pointSize * 0.01f,
            glm::vec2(0.035f, 0.05f),
            this->warningText.c_str()
          )
        );
      }

      this->depthZ = bb::GenerateMesh(
        zpoints
      );
    }
    namespace
    {
      const glm::vec2 engineTri[] = 
      {
        glm::vec2( -0.1f,  -0.05f),
        glm::vec2( -0.1f,   0.05f),
        glm::vec2(  0.0f,   0.00f),
        glm::vec2( -0.1f,  -0.05f)
      };

      const glm::vec2 rudderTri[] = 
      {
        glm::vec2(-0.05f,   0.1f),
        glm::vec2( 0.05f,   0.1f),
        glm::vec2( 0.00f,  -0.0f),
        glm::vec2(-0.05f,   0.1f)
      };

      bb::meshDesc_t EngineTriangle(float pointSize, float normEngineOutput, glm::vec2 scale)
      {
        auto points = bb::linePoints_t();

        auto tmap = 
          glm::scale(
            glm::translate(
              glm::mat3(1.0f),
              glm::vec2(-0.85f, 0.8f - normEngineOutput)
            ),
            glm::vec2(0.35f, 0.5f) * scale
          );

        for (auto triPoint: engineTri)
        {
          points.emplace_back(
            tmap*glm::vec3(triPoint, 1.0f)
          );
        }

        return bb::DefineLine(glm::vec3(0.0f), pointSize * 0.02f, points);
      }

      bb::meshDesc_t BallastTriangle(float pointSize, float normBallastOutput, glm::vec2 scale)
      {
        auto points = bb::linePoints_t();

        auto tmap = 
          glm::scale(
            glm::translate(
              glm::mat3(1.0f),
              glm::vec2(0.8f, 0.8f - normBallastOutput)
            ),
            glm::vec2(0.35f, 0.5f) * scale
          );

        for (auto triPoint: engineTri)
        {
          points.emplace_back(
            tmap*glm::vec3(triPoint, 1.0f)
          );
        }

        return bb::DefineLine(glm::vec3(0.0f), pointSize * 0.02f, points);
      }

      bb::meshDesc_t RudderTriangle(float pointSize, float normRudderOutput, glm::vec2 scale)
      {
        auto points = bb::linePoints_t();

        auto tmap = 
          glm::scale(
            glm::translate(
              glm::mat3(1.0f),
              glm::vec2(-0.775f - normRudderOutput, -0.8f)
            ),
            glm::vec2(0.35f, 0.5f) * scale
          );

        for (auto triPoint: rudderTri)
        {
          points.emplace_back(
            tmap*glm::vec3(triPoint, 1.0f)
          );
        }

        return bb::DefineLine(glm::vec3(0.0f), pointSize * 0.02f, points);
      }

    }

    void screen_t::UpdateRudder(const state_t& state)
    {
      auto maxRudder = rudder::Output(rudder::left_40);
      auto normRudder = state.Player().rudderPos/maxRudder*0.175f;
      auto destRudder = rudder::Output(state.Player().rudder)/maxRudder*0.175f;

      auto actualRudderTri = RudderTriangle(this->pointSize * 0.02f, normRudder, glm::vec2(1.0f));

      if (this->speedMult == 1)
      {
        actualRudderTri.Append(
          RudderTriangle(this->pointSize * 0.02f, destRudder, glm::vec2(1.0f, -1.0f))
        );
      }

      this->rudder = bb::GenerateMesh(
        actualRudderTri
      );
    }

    void screen_t::UpdateEngine(const state_t& state)
    {
      auto maxEngine = state.Player().engineModeList.Output(sub3000::engine::mode_t::full_ahead);
      auto normEngineOutput = (state.Player().engineOutput/maxEngine)*0.15f;
      auto destEngineOutput = state.Player().engineModeList.Output(state.Player().engine)/maxEngine*0.15f;

      auto actualOutputTri = EngineTriangle(this->pointSize * 0.02f, normEngineOutput, glm::vec2(1.0f));
      if (this->speedMult == 1)
      {
        actualOutputTri.Append(
          EngineTriangle(this->pointSize * 0.02f, destEngineOutput, glm::vec2(-1.0f, 1.0f))
        );
      }

      this->engine = bb::GenerateMesh(
        actualOutputTri
      );
    }

    void screen_t::UpdateBallast(const state_t& state)
    {
      auto maxBallast = ballast::Output(ballast::blow);
      auto normBallastOutput = (state.Player().ballastStatus/maxBallast)*0.15f;
      auto destBallastOutput = ballast::Output(state.Player().ballast)/maxBallast*0.15f;

      auto actualOutputTri = BallastTriangle(this->pointSize * 0.02f, normBallastOutput, glm::vec2(1.0f));
      if (this->speedMult == 1)
      {
        actualOutputTri.Append(
          BallastTriangle(this->pointSize * 0.02f, destBallastOutput, glm::vec2(-1.0f, 1.0f))
        );
      }

      this->ballast = bb::GenerateMesh(
        actualOutputTri
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
        this->speedMult = state->simSpeed;
        if (this->speedMult != 1)
        {
          this->speedMultMesh = bb::GenerateMesh(
            bb::DefineNumber(
              glm::vec3(-0.95f, -0.7f, 0.0f),
              0.01f, 
              glm::vec2(0.10f),
              (std::to_string(state->simSpeed) + "x").c_str()
            )
          );
        }

        this->UpdateUnits(
          std::move(state->Units()),
          state->RadarAngle(),
          static_cast<float>(dt)
        );
        this->UpdateDepthRadar(*state);

        this->UpdateRudder(*state);
        this->UpdateEngine(*state);
        this->UpdateBallast(*state);

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

        this->hasCollision = state->Player().hasCollision;
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

      this->shader.SetVector4f(
        "lineColor",
        (this->hasCollision)
          ?glm::vec4(1.3f, 0.1f, 0.1f, 1.0f)
          :glm::vec4(0.1f, 1.3f, 0.1f, 1.0f)
      );

      this->radar.Render();
      if (this->units.Good())
      {
        this->units.Render();
      }
      this->radarLine.Render();

      if (this->rudder.Good())
      {
        this->rudder.Render();
      }
      if (this->engine.Good())
      {
        this->engine.Render();
      }
      if (this->ballast.Good())
      {
        this->ballast.Render();
      }
      if (this->speedMultMesh.Good() && (this->speedMult != 1))
      {
        this->speedMultMesh.Render();
      }

      this->shader.SetVector4f(
        "lineColor",
        (this->mayCollide)
          ?glm::vec4(1.3f, 0.1f, 0.1f, 1.0f)
          :glm::vec4(0.1f, 1.3f, 0.1f, 1.0f)
      );
      if (this->depthZ.Good())
      {
        this->depthZ.Render();
      }
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