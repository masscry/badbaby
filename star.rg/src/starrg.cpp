#include <starrg.hpp>

#include <common.hpp>
#include <mailbox.hpp>
#include <shader.hpp>
#include <camera.hpp>
#include <shapes.hpp>
#include <image.hpp>
#include <worker.hpp>
#include <font.hpp>
#include <text.hpp>

#include <sstream>

std::mt19937& RandomEngine()
{
  static std::random_device rd;
  static std::mt19937 gen(rd());
  return gen;
}

const size_t maxLines = 44;

void starrg_t::Update(double dt)
{
  bb::msg_t msg;
  while (this->box->Poll(&msg))
  {
    if (auto tileDesc = bb::As<meshData_t>(msg))
    {
      switch(tileDesc->type)
      {
        case meshData_t::M_MAP:
          this->map = bb::GenerateMesh(
            tileDesc->Data()
          );
          break;
        case meshData_t::M_UNIT:
          this->unit = bb::GenerateMesh(
            tileDesc->Data()
          );
          this->time = 0.5;
          break;
      }
      continue;
    }
    if (auto log = bb::As<bb::msg::dataMsg_t<std::string>>(msg))
    {
      logLines.emplace_back(std::move(log->Data()));
      if (logLines.size() > maxLines)
      {
        logLines.pop_front();
      }

      std::stringstream linesText;
      linesText << "\\0";

      for(auto it = logLines.begin(), e = logLines.end()-1; it != e; ++it)
      {
        linesText << *it << "\n";
      }
      if (logLines.size() == maxLines)
      {
        linesText << "\\1" << *(logLines.end()-1) << "\n";
      }
      else
      {
        linesText << *(logLines.end()-1) << "\n";
      }

      this->logText = linesText.str();

      continue;
    }
    // Unknown message!
    assert(0);
  }

  if (!this->logText.empty())
  {
    this->log.Update("%s", this->logText.c_str());
  }
  this->text.Update("%5.1f", 1.0/dt);
  if (this->time > 0.0)
  {
    this->time -= dt;
  }
  else 
  {
    this->time = 0.0;
  }
}

void starrg_t::Render()
{
  bb::framebuffer_t::Bind(this->context.Canvas());

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDepthFunc(GL_LESS);
  
  this->camera.Update();

  bb::shader_t::Bind(this->mapShader);
  this->mapShader.SetBlock("camera", camera.UniformBlock());
  this->mapShader.SetFloat(
    "time",
    static_cast<float>(this->time)
  );

  bb::texture_t::Bind(tileset);
  if (this->map.Good())
  {
    this->mapShader.SetFloat(
      "minAlpha",
      1.0f
    );
    this->map.Render();
  }

  if (this->unit.Good())
  {
    this->mapShader.SetFloat(
      "minAlpha",
      0.0f
    );
    this->unit.Render();
  }

  glDepthFunc(GL_ALWAYS);
  bb::shader_t::Bind(this->fontShader);
  this->fontShader.SetBlock("camera", camera.UniformBlock());
  
  this->fontShader.SetMatrix(
    "model",
    glm::mat4(1.0f)
  );
  this->text.Render();
  
  this->fontShader.SetMatrix(
    "model",
    glm::translate(glm::mat4(1.0f), glm::vec3(720.0f, 0.0f, 0.0f))
  );
  if (!this->logText.empty())
  {
    this->log.Render();
  }
  bb::framebuffer_t::RenderToScreen();
}

starrg_t::starrg_t(bb::context_t& context)
: context(context),
  camera(bb::camera_t::Orthogonal(
    0.0f,
    static_cast<float>(context.Width()),
    static_cast<float>(context.Height()),
    0.0f)
  ),
  mapShader(bb::shader_t::LoadProgramFromFiles("starrg.vp.glsl", "starrg.fp.glsl")),
  fontShader(bb::shader_t::LoadProgramFromFiles("font.vp.glsl", "font.fp.glsl")),
  tileset(bb::texture_t::LoadConfig("tiles.config")),
  box(bb::postOffice_t::Instance().New("StarView"))
{
  auto len = static_cast<int>(context.Height()/tileSize.y);

  this->world = bb::workerPool_t::Instance().Register<world_t>(glm::ivec2(len));
  this->context.RegisterActorCallback(
    this->world,
    bb::context_t::keyboard
  );

  this->font.Load("font.config");
  this->text = bb::textDynamic_t(this->font, bb::vec2_t(8.0f, -16.0f));
  this->log = bb::textDynamic_t(this->font, bb::vec2_t(8.0f, -16.0f));
  glDisable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
}

starrg_t::~starrg_t()
{
  bb::workerPool_t::Instance().Unregister(this->world);
}
