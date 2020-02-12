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
  bb::mailbox_t mail;
}

namespace paint
{

  void PostToMain(bb::msg_t msg)
  {
    mail.Put(msg);
  }

  const char* MainMsgToStr(const bb::msg_t& msg)
  {
    switch(msg.type)
    {
      case bb::msgID_t::KEYBOARD:
        return "key";
      case mainMessage_t::update:
        return "update";
      case mainMessage_t::nop:
        return "nop";
      case mainMessage_t::exit:
        return "exit";
      default:
        return "???";
    }
  }

}

class monitorOpenedFile_t final: public bb::fs::processor_t
{
public:
  int OnChange(const char*, bb::fs::event_t)
  {
    paint::PostToMain(bb::MakeMsg(-1, paint::mainMessage_t::update, 0));
    return 0;
  }
};

const char* vpShader = R"shader(
  // Vertex Shader
  #version 330 core

  layout(location = 0) in vec2 pos;
  layout(location = 1) in float dist;

  uniform camera
  {
    mat4 proj;
    mat4 view;
  };

  uniform mat4 model;

  out float fragPos;

  void main()
  {
    fragPos = dist;
    gl_Position = proj * view * model * vec4(pos, 0.0f, 1.0f);
  }
)shader";

const char* fpShader = R"shader(
  // Fragment shader
  #version 330 core

  layout(location = 0) out vec4 pixColor;

  in float fragPos;

  const vec4 lineColor = vec4(0.1f, 1.3f, 0.1f, 1.0f);

  void main()
  {
    float fragDist = 1.0-2.0*abs(fragPos - 0.5);
    pixColor = mix(vec4(0.0f), lineColor, fragDist);
  }
)shader";

std::list<std::pair<bb::mesh_t,bb::node_t>> meshes;
bb::shader_t lineShader;
bb::camera_t camera;

class painterVM_t: public bb::vm_t
{
  float brushWidth;
  bb::node_t cursor;
  uint32_t sides;

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
      this->cursor.Reset();
      /* FALLTHROUGH */
    case 'r':
      this->cursor.Translate(
        bb::vec3_t(
          static_cast<float>(bb::Argument(refs, 0)),
          static_cast<float>(bb::Argument(refs, 1)),
          0.0f
        )
      );
      break;
    case 'l':
      {
        bb::linePoints_t linePoints;

        linePoints.emplace_back(0.0f, 0.0f);
        linePoints.emplace_back(
          static_cast<float>(bb::Argument(refs, 0)),
          static_cast<float>(bb::Argument(refs, 1))
        );

        meshes.push_back(
          std::make_pair(
            bb::GenerateLine(
              this->brushWidth,
              linePoints
            ),
            this->cursor
          )
        );
      }
      break;
    case 'b':
      this->brushWidth = static_cast<float>(bb::Argument(refs, 0));
      break;
    case 'c':
      meshes.push_back(
        std::make_pair(
	  bb::GenerateCircle(
            this->sides,
            static_cast<float>(bb::Argument(refs, 0)),
            this->brushWidth
          ),
          this->cursor
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

  painterVM_t()
  : brushWidth(0.0f),
    sides(32)
  {
    ;
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

  for (auto& item: meshes)
  {
    lineShader.SetMatrix(
      "model",
      item.second.Model()
    );
    item.first.Render();
  }

}

int UpdateScene(const char* scriptName)
{
  auto script = bb::ReadWholeFile(scriptName, nullptr);
  if (script == nullptr)
  {
    return -1;
  }
  BB_DEFER(free(script));

  meshes.clear();

  painterVM_t painterVM;
  if (bb::ExecuteScript(painterVM, script) != 0)
  {
    return -1;
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
    meshes.clear();
    camera = bb::camera_t();
    lineShader = bb::shader_t();
  );

  auto& context = bb::context_t::Instance();
  lineShader = bb::shader_t(vpShader, fpShader);

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

    if (mail.Poll(&msgToMain))
    {
      bb::Debug(
        "Got \"%s\" (%u) message",
        paint::MainMsgToStr(msgToMain),
        msgToMain.type
      );
      switch(msgToMain.type)
      {
        case paint::nop:
          break;
        case paint::update:
          if (UpdateScene(argv[1]) != 0)
          {
            bb::Error("%s", "Invalid PVF");
            fprintf(stderr, "%s\n", "Invalid PVF");
          }
          break;
        case paint::exit:
          loop = false;
          break;
        default:
          assert(0);
      }
    }
  }

  fprintf(stdout, "%s\n", "Bye-bye!");
  return 0;
}
