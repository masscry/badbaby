#include <random>
#include <memory>
#include <cmath>

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <common.hpp>
#include <context.hpp>
#include <simplex.hpp>
#include <shapes.hpp>
#include <camera.hpp>

#include <mapGen.hpp>
#include <worker.hpp>
#include <monfs.hpp>

class newConfig_t: public bb::msg::basic_t
{
public:
  newConfig_t()
  : bb::msg::basic_t(-1)
  {
    ;
  }
};

class newShader_t: public bb::msg::basic_t
{
public:
  newShader_t()
  : bb::msg::basic_t(-1)
  {
    ;
  }
};

class updateConfig_t: public bb::fs::processor_t
{
public:

  int OnChange(const char* fname, bb::fs::event_t)
  {
    if (strcmp(fname, "006simplex.config") == 0)
    {
      bb::postOffice_t::Instance().Post("simplex",
        bb::Issue<newConfig_t>()
      );
      return 0;
    }
    if (strcmp(fname, "006simplex_fp.glsl") == 0)
    {
      bb::postOffice_t::Instance().Post("simplex",
        bb::Issue<newShader_t>()
      );
      return 0;
    }
    if (strcmp(fname, "006simplex_vp.glsl") == 0)
    {
      bb::postOffice_t::Instance().Post("simplex",
        bb::Issue<newShader_t>()
      );
      return 0;
    }
    return 0;
  }

};

int main(int argc, char* argv[])
{
  auto mailbox = bb::postOffice_t::Instance().New("simplex");

  if (bb::ProcessStartupArguments(argc, argv) != 0)
  {
    return -1;
  }

  auto& context = bb::context_t::Instance();

  auto mapGenActor = bb::workerPool_t::Instance().Register<bb::ext::mapGen_t>();
  if (mapGenActor == -1)
  {
    bb::Error("%s", "Can't create bb::ext::mapGen_t actor");
    return -1;
  }
  BB_DEFER(bb::workerPool_t::Instance().Unregister(mapGenActor));

  auto fsmon = bb::fs::monitor_t::Create<updateConfig_t>();
  fsmon.Watch("./");

  mailbox->Put(bb::Issue<newConfig_t>());

  auto plane = bb::GeneratePlane(glm::vec2(100.0f, 100.0f), glm::vec3(0.0f), bb::vec2_t(0.5f), false);

  auto renderProgram = bb::shader_t::LoadProgramFromFiles(
    "006simplex_vp.glsl",
    "006simplex_fp.glsl"
  );

  float zNearVal = 0.1f;
  float zFarVal = 1000.0f;

  auto worldCamera = bb::camera_t::Perspective(45.0f, context.AspectRatio(), zNearVal, zFarVal);
  auto bindCameraPoint = renderProgram.UniformBlockIndex("camera");

  worldCamera.View() = glm::lookAt(
    glm::vec3(0.0f, 0.0f, 80.0f),
    glm::vec3(0.0f, 0.0f, -1.0f),
    glm::vec3(0.0f, 1.0f, 0.0f)
  );

  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

  auto lastTick = glfwGetTime();

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  bb::texture_t tex;

  uint16_t texWidth = 1;
  uint16_t texHeight = 1;

  for(;;)
  {
    auto nowTick = glfwGetTime();
    auto delta = nowTick - lastTick;
    lastTick = nowTick;

    fsmon.Check();

    bb::msg_t msg;
    if (mailbox->Poll(&msg))
    {
      if (auto newShaderMsg = bb::As<newShader_t>(msg))
      {
        renderProgram = bb::shader_t::LoadProgramFromFiles(
          "006simplex_vp.glsl",
          "006simplex_fp.glsl"
        );
      }

      if (auto newConfigMsg = bb::As<newConfig_t>(msg))
      {
        auto config = bb::config_t("006simplex.config");

        auto maxWidth = static_cast<uint16_t>(config.Value("map.width", 2048.0));
        auto maxHeight = static_cast<uint16_t>(config.Value("map.height", 2048.0));

        auto mapSeed = static_cast<int64_t>(config.Value("map.seed", 0.0));
        auto mapRadiusStart = static_cast<float>(config.Value("map.radius.start", 1.0));
        auto mapRadiusFinish = static_cast<float>(config.Value("map.radius.finish", 10.0));
        auto mapRadiusRounds = static_cast<size_t>(config.Value("map.radius.rounds", 10.0));
        auto mapFalloff = static_cast<float>(config.Value("map.falloff", 0.2));
        auto mapPower = static_cast<float>(config.Value("map.power", 2.0));

        uint16_t cWidth = 64;
        uint16_t cHeight = 64;

        do
        {
          if (cWidth < maxWidth)
          {
            cWidth <<= 1;
          }
          if (cHeight < maxHeight)
          {
            cHeight <<= 1;
          }
          bb::Debug("[%u; %u]", cWidth, cHeight);

          bb::workerPool_t::Instance().PostMessage(
            mapGenActor,
            bb::Issue<bb::ext::generate_t>(
              mailbox->Address(),
              cWidth,
              cHeight,
              mapRadiusStart,
              mapRadiusFinish,
              mapSeed,
              mapFalloff,
              mapRadiusRounds,
              mapPower
            )
          );
        } while ((cWidth < maxWidth) || (cHeight < maxHeight));
      }

      if (auto mapDataMsg = bb::As<bb::ext::hmDone_t>(msg))
      {
        texWidth = mapDataMsg->HeightMap().Width();
        texHeight = mapDataMsg->HeightMap().Height();
        tex = bb::texture_t(mapDataMsg->HeightMap().Width(), mapDataMsg->HeightMap().Height(), mapDataMsg->HeightMap().Data());
        tex.GenerateMipmaps();
        tex.SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
      }
    }
    else
    {
      bb::framebuffer_t::Bind(context.Canvas());
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      bb::shader_t::Bind(renderProgram);
      worldCamera.Update();
      renderProgram.SetBlock(bindCameraPoint, worldCamera.UniformBlock());
      renderProgram.SetFloat(
        "time", static_cast<float>(nowTick)
      );

      bb::texture_t::Bind(tex);
      plane.Render();
    }

    if (!context.Update())
    {
      break;
    }

    context.Title(
      std::to_string((int)round(1.0/delta)) 
      + " ["+ std::to_string(texWidth) + ";" + std::to_string(texHeight) + "]"
    );

    glm::vec3 offset(
      -(context.IsKeyDown(GLFW_KEY_D)-context.IsKeyDown(GLFW_KEY_A))*delta*10,
      -(context.IsKeyDown(GLFW_KEY_W)-context.IsKeyDown(GLFW_KEY_S))*delta*10,
      0.0f
    );

    worldCamera.View() = glm::translate(worldCamera.View(), offset);

  }

  return 0;
}
