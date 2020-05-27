#include <string>

#include <painter.hpp>
#include <camera.hpp>
#include <common.hpp>
#include <mailbox.hpp>
#include <context.hpp>
#include <shapes.hpp>
#include <shader.hpp>
#include <script.hpp>
#include <actor.hpp>
#include <worker.hpp>
#include <role.hpp>
#include <monfs.hpp>

namespace
{
  bb::mailbox_t::shared_t mail = bb::postOffice_t::Instance().New("painter");
}

namespace paint
{

  void PostToMain(bb::msg_t&& msg)
  {
    mail->Put(std::move(msg));
  }

  const char* MainMsgToStr(const bb::msg_t& msg)
  {
    if (bb::As<bb::msg::keyEvent_t>(msg) != nullptr)
    {
      return "key";
    }
    if (bb::As<paint::update_t>(msg) != nullptr)
    {
      return "update";
    }
    if (bb::As<paint::exit_t>(msg) != nullptr)
    {
      return "exit";
    }
    return "???";
  }

}

class monitorOpenedFile_t final: public bb::fs::processor_t
{
public:
  int OnChange(const char*, bb::fs::event_t)
  {
    paint::PostToMain(
      bb::Issue<paint::update_t>()
    );
    return 0;
  }
};

const char* vpShader = R"shader(
  // Vertex Shader
  #version 330 core

  layout(location = 0) in vec2 pos;
  layout(location = 1) in vec2 dist;

  uniform camera
  {
    mat4 proj;
    mat4 view;
  };

  out vec2 fragPos;

  void main()
  {
    fragPos = dist;
    gl_Position = proj * view * vec4(pos, 0.0f, 1.0f);
  }
)shader";

const char* fpShader =
  R"shader(
  // Fragment shader
  #version 330 core

  layout(location = 0) out vec4 pixColor;

  in vec2 fragPos;

  const vec4 lineColor = vec4(0.1f, 1.3f, 0.1f, 1.0f);

  void main()
  {
    float pct = 1.0f - 2.0f*distance(fragPos, vec2(0.5));
    pixColor = mix(vec4(0.0f), lineColor, pct);
  }
)shader";

bb::mesh_t mesh;
bb::shader_t lineShader;
bb::camera_t camera;

class painterVM_t: public bb::vm_t
{
  float brushWidth;
  glm::vec3 cursor;
  uint32_t sides;
  bb::meshDesc_t meshDesc;
  glm::vec2 textScale;

  int OnCommand(int cmd, const bb::listOfRefs_t& refs) override
  {
    switch(cmd)
    {
    case 'f':
    {
      camera = bb::camera_t::Orthogonal(
        static_cast<float>(bb::Argument(refs, 2)),
        static_cast<float>(bb::Argument(refs, 0)),
        static_cast<float>(bb::Argument(refs, 1)),
        static_cast<float>(bb::Argument(refs, 3))
      );
      break;
    }
    case 'm':
      this->cursor = glm::vec3(0.0f);
      /* FALLTHROUGH */
    case 'r':
      this->cursor.x += static_cast<float>(bb::Argument(refs, 0));
      this->cursor.y += static_cast<float>(bb::Argument(refs, 1));
      break;
    case 'l':
      {
        bb::linePoints_t linePoints;

        linePoints.emplace_back(this->cursor);
        this->cursor.x = static_cast<float>(bb::Argument(refs, 0));
        this->cursor.y = static_cast<float>(bb::Argument(refs, 1));

        linePoints.emplace_back(this->cursor);

        this->meshDesc.Append(
          bb::DefineLine(
            glm::vec3(0.0f),
            this->brushWidth,
            linePoints
          )
        );
      }
      break;
    case 'b':
      this->brushWidth = static_cast<float>(bb::Argument(refs, 0));
      break;
    case 'c':
      this->meshDesc.Append(
        bb::DefineCircle(
          this->cursor,
          this->sides,
          static_cast<float>(bb::Argument(refs, 0)),
          this->brushWidth
        )
      );
      break;
    case 's':
      this->sides = static_cast<uint32_t>(bb::Argument(refs, 0));
      if (this->sides == 0)
      {
        this->sides = 32;
      }
      break;
    case 't':
      this->textScale.x = static_cast<float>(bb::Argument(refs, 0));
      this->textScale.y = static_cast<float>(bb::Argument(refs, 1));
      break;
    case 'd':
      this->meshDesc.Append(
        bb::DefineNumber(
          this->cursor,
          this->brushWidth,
          this->textScale,
          bb::StringArg(refs, 0).c_str()
        )
      );
      break;
    default:
      bb::Debug("Command %c (%d)\n", cmd, cmd);
      for (auto& item: refs)
      {
        bb::Debug("\t%f\n", item.Number());
      }
    }
    return 0;
  }

public:

  const bb::meshDesc_t& GetMeshDescription() const
  {
    return this->meshDesc;
  }

  bb::mesh_t GetMesh() const
  {
    return bb::GenerateMesh(this->meshDesc);
  }

  painterVM_t()
  : brushWidth(0.0f),
    cursor(0.0f),
    sides(32),
    textScale(1.0f)
  {
    this->meshDesc.SetDrawMode(GL_TRIANGLE_STRIP);
  }

  painterVM_t(const painterVM_t&) = delete;
  painterVM_t& operator=(const painterVM_t&) = delete;
  painterVM_t(painterVM_t&&) = delete;
  painterVM_t& operator=(painterVM_t&&) = delete;

};

void Render()
{
  auto& context = bb::context_t::Instance();
  bb::framebuffer_t::Bind(context.Canvas());

  glBlendFunc(
    GL_ONE, GL_ONE_MINUS_SRC_ALPHA
  );

  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  bb::shader_t::Bind(lineShader);
  camera.Update();

  lineShader.SetBlock(
    lineShader.UniformBlockIndex("camera"),
    camera.UniformBlock()
  );

  mesh.Render();
}

int UpdateScene(const char* scriptName)
{
  FILE* input = fopen(scriptName, "rt");
  if (input == nullptr)
  {
    return -1;
  }
  BB_DEFER(fclose(input));

  if (bb::meshDesc_t::CheckFile(input))
  {
    auto meshDesc = bb::meshDesc_t::Load(input);
    mesh = bb::GenerateMesh(meshDesc);

    camera = bb::camera_t::Orthogonal(
      -1.0f, 1.0f, 1.0f, -1.0f
    );

  }
  else
  {
    auto script = bb::ReadWholeFile(input, nullptr);
    if (script == nullptr)
    {
      return -1;
    }
    BB_DEFER(free(script));

    painterVM_t painterVM;
    if (bb::ExecuteScript(painterVM, script) != 0)
    {
      return -1;
    }
    mesh = painterVM.GetMesh();

    FILE* output = fopen((std::string(scriptName) + ".msh").c_str(), "wb");
    if (output != nullptr)
    {
      BB_DEFER(fclose(output));
      bb::Info("Saving mesh description...");
      bb::Info(
        "Save mesh result: %d",
        painterVM.GetMeshDescription().Save(output)
      );
    }
  }
  return 0;
}

int main(int argc, char* argv[])
{
  if (bb::ProcessStartupArguments(argc, argv) != 0)
  {
    return -1;
  }

  if (argc != 2)
  {
    fprintf(stderr, "Error: No script provided!\n");
    fprintf(stderr, "Usage: painter SCRIPT\n");
    return -1;
  }

  //
  // These objects are global, must be explicitly deleted
  // before context_t expires.
  //
  // context_t expires some time after main exits, so
  // can safely delete on main scope exit.
  //
  BB_DEFER(
    mesh = bb::mesh_t();
    camera = bb::camera_t();
    lineShader = bb::shader_t();
  );

  auto& context = bb::context_t::Instance();

  try
  {
    lineShader = bb::shader_t(vpShader, fpShader);
  }
  catch (const std::runtime_error& shaderError)
  {
    fprintf(stderr, "%s. Abort.\n", shaderError.what());
    return -1;
  }

  if (UpdateScene(argv[1]) != 0)
  {
    fprintf(stderr, "%s\n", "Invalid PVF. Abort.");
    return -1;
  }

  bb::fs::monitor_t monitor = bb::fs::monitor_t::Create(
    std::unique_ptr<bb::fs::processor_t>(new monitorOpenedFile_t())
  );

  monitor.Watch(argv[1]);

  bool loop = true;
  while(loop)
  {
    Render();

    if (!context.Update())
    {
      break;
    }

    monitor.Check();

    bb::msg_t msgToMain;

    if (mail->Poll(&msgToMain))
    {
      if (auto* defMsg = msgToMain.get())
      {
        bb::Debug(
          "Got \"%s\" (%lu) message",
          paint::MainMsgToStr(msgToMain),
          typeid(*defMsg).hash_code()
        );

        if (bb::msg::As<paint::update_t>(*defMsg))
        {
          if (UpdateScene(argv[1]) != 0)
          {
            bb::Error("%s", "Invalid PVF");
            fprintf(stderr, "%s\n", "Invalid PVF");
          }
          continue;
        }

        if (bb::msg::As<paint::exit_t>(*defMsg))
        {
          loop = false;
          continue;
        }

        bb::Error("Unknown message: %s", typeid(*defMsg).name());
      }
      else
      {
        bb::Error("Unknown message: %s", "Invalid Message");
      }

      assert(0);
    }
  }

  fprintf(stdout, "%s\n", "Bye-bye!");
  return 0;
}
