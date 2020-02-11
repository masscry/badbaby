#include <painter.hpp>

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

  out float fragPos;

  void main()
  {
    fragPos = dist;
    gl_Position = vec4(pos, 0.0f, 1.0f);
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

bb::mesh_t circle;
bb::shader_t lineShader;

class painterVM_t: public bb::vm_t
{
  float brushWidth;

  int OnCommand(int cmd, const bb::listOfRefs_t& refs) override
  {
    switch(cmd)
    {
    case 'b':
      this->brushWidth = static_cast<float>(bb::Argument(refs, 0));
      break;
    case 'c':
      circle = bb::GenerateCircle(
        32,
        static_cast<float>(bb::Argument(refs, 0)),
        this->brushWidth
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

  painterVM_t()
  :brushWidth(0.0f)
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
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  bb::shader_t::Bind(lineShader);
  circle.Render();
}

int UpdateScene(const char* scriptName)
{
  auto script = bb::ReadWholeFile(scriptName, nullptr);
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
  circle = bb::mesh_t();

  return 0;
}
