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
      case mainMessage_t::nop:
        return "nop";
      case mainMessage_t::exit:
        return "exit";
      default:
        return "???";
    }
  }

}

class passToMainLoop_t final: public bb::role_t
{

  bb::msgResult_t OnProcessMessage(const bb::actor_t&, bb::msg_t msg) override
  {
    paint::PostToMain(msg);
    return bb::msgResult_t::complete;
  }

public:

  const char* DefaultName() const override
  {
    return "passToMainLoop";
  }

  passToMainLoop_t()
  {
    ;
  }

  ~passToMainLoop_t() override
  {
    ;
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

  const vec4 lineColor = vec4(0.1f, 1.0f, 0.1f, 1.0f);

  void main()
  {
    float fragDist = 1.0-2.0*abs(fragPos - 0.5);
    pixColor = lineColor*fragDist;
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
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
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

  auto& context = bb::context_t::Instance();
  lineShader = bb::shader_t(vpShader, fpShader);

  if (UpdateScene(argv[1]) != 0)
  {
    return -1;
  }

  auto& pool = bb::workerPool_t::Instance();
  auto passToMainLoopActor = pool.Register(
    std::unique_ptr<bb::role_t>(new passToMainLoop_t())
  );
  context.RegisterActorCallback(passToMainLoopActor, bb::cmfKeyboard);

  bool loop = true;
  while(loop)
  {
    Render();

    if (!context.Update())
    {
      break;
    }

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
        case bb::msgID_t::KEYBOARD:
          {
            auto keyEvent = bb::GetMsgData<bb::keyEvent_t>(msgToMain);
            if ((keyEvent.press != GLFW_PRESS) && (keyEvent.key == GLFW_KEY_F5))
            {
              UpdateScene(argv[1]);
            }
          }
          break;
        case paint::nop:
          break;
        case paint::exit:
          loop = false;
          break;
        default:
          assert(0);
      }
    }
  }

  pool.Unregister(passToMainLoopActor);

  lineShader = bb::shader_t();
  meshes.clear();
  camera = bb::camera_t();

  return 0;
}
