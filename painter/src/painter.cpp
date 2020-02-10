#include <painter.hpp>

#include <common.hpp>
#include <mailbox.hpp>
#include <context.hpp>
#include <shapes.hpp>
#include <shader.hpp>

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

void Render()
{
  auto& context = bb::context_t::Instance();
  bb::framebuffer_t::Bind(context.Canvas());
  bb::shader_t::Bind(lineShader);
  circle.Render();
}

int main(int argc, char* argv[])
{
  if (bb::ProcessStartupArguments(argc, argv) != 0)
  {
    return -1;
  }

  auto& context = bb::context_t::Instance();

  circle = bb::GenerateCircle(3, 0.5f, 0.1f);
  lineShader = bb::shader_t(vpShader, fpShader);

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

  lineShader = bb::shader_t();
  circle = bb::mesh_t();

  return 0;
}
