#include <demo.hpp>
#include <space.hpp>

#include <context.hpp>
#include <camera.hpp>
#include <worker.hpp>
#include <meshDesc.hpp>
#include <shapes.hpp>

namespace sub3000
{

  void demoScene_t::OnPrepare()
  {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glDisable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    auto dims = bb::context_t::Instance().Dimensions();

    this->camera = bb::camera_t::Orthogonal(
      -dims.x/20.0f,
      dims.x/20.0f,
      dims.y/20.0f,
      -dims.y/20.0f
    );

    this->shader = bb::shader_t::LoadProgramFromFiles(
      "demo.vp.glsl",
      "demo.fp.glsl"
    );

    this->box = bb::postOffice_t::Instance().New("arenaScreen");

    this->space = bb::workerPool_t::Instance().Register<sub3000::space_t>();
    bb::context_t::Instance().RegisterActorCallback(
      this->space,
      bb::cmfKeyboard
    );

    if (auto worldMapFile = bb::ext::binstore_t::Read("world.bbw"))
    {
      auto worldMap = bb::ext::distanceMap_t(worldMapFile);
      if (worldMap.IsGood())
      {
        bb::workerPool_t::Instance().PostMessage(
          this->space,
          bb::Issue<bb::ext::hmDone_t>(
            -1, 
            bb::ext::heightMap_t(worldMap.HeightMap()),
            std::move(worldMap)
          )
        );
      }

      bb::meshDesc_t lineDesc;

      auto wmDim = worldMap.Dimensions();
      for (float i = 0.0f; i < wmDim.x; i += 10.0f)
      {
        lineDesc.Append(
          bb::DefineLine(glm::vec3(0.0f), 1.0f, 
          bb::linePoints_t{
            glm::vec2(i, 0.0f),
            glm::vec2(i, wmDim.y)
          })
        );
      }

      for (float i = 0.0f; i < wmDim.y; i += 10.0f)
      {
        lineDesc.Append(
          bb::DefineLine(glm::vec3(0.0f), 1.0f, 
          bb::linePoints_t{
            glm::vec2(0.0f,    i),
            glm::vec2(wmDim.x, i)
          })
        );
      }

      this->lines = bb::GenerateMesh(lineDesc);
    }
    else
    {
      assert(0);
      bb::Error("%s", "Map Read Failed!");
      sub3000::PostExit();
    }

  }

  void demoScene_t::OnUpdate(double delta)
  {
    bb::workerPool_t::Instance().PostMessage(
      this->space,
      bb::msg_t(new step_t(this->box->Address(), delta))
    );

    auto msg = this->box->Wait();
    if (auto state = bb::As<state_t>(msg))
    {
      if (!state->Units().empty())
      {
        this->camera.View() = glm::translate(
          glm::rotate(
            glm::mat4(1.0f),
            state->Angle(),
            glm::vec3(0.0f, 0.0f, 1.0f)
          ),
          glm::vec3(-state->Pos(), 0.0f)
        );

        auto dir = bb::Dir(state->Angle());
        auto meshDesc = bb::DefinePoints(1.0f, state->Units());

        meshDesc.Append(
          bb::DefineLine(glm::vec3(0.0f), 1.0f, 
          bb::linePoints_t{
            state->Pos(),
            state->Pos() + dir*10.0f}
          )
        );

        this->mesh = bb::GenerateMesh(meshDesc);
      }
    }
  }

  void demoScene_t::OnRender()
  {
    bb::framebuffer_t::Bind(bb::context_t::Instance().Canvas());

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glBlendFunc(
      GL_SRC_ALPHA, GL_ONE
    );

    bb::shader_t::Bind(this->shader);
    camera.Update();

    this->shader.SetBlock(
      this->shader.UniformBlockIndex("camera"),
      this->camera.UniformBlock()
    );
    if (this->lines.Good())
    {
      this->lines.Render();
    }
    if (this->mesh.Good())
    {
      this->mesh.Render();
    }
  }

  void demoScene_t::OnCleanup()
  {
    bb::workerPool_t::Instance().Unregister(this->space);
    this->box.reset();
  }

  demoScene_t::demoScene_t()
  : scene_t(sceneID_t::demo, "Demo")
  {
    ;
  }

}