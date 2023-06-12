
#include <starrg.hpp>

namespace
{
  bb::mailbox_t::shared_t& Mailbox()
  {
    static auto mail = bb::postOffice_t::Instance().New("starrg");
    return mail;
  }
}

int main(int argc, char* argv[])
{
  if (bb::ProcessStartupArguments(argc, argv) != 0)
  {
    return EXIT_FAILURE;
  }

  auto& context = bb::context_t::Instance();
  bb::workerPool_t::Instance();
  auto loop = true;
  bb::msg_t msg;
  auto lastTick = glfwGetTime();
  starrg_t starrg(context);

  while(loop)
  {
    auto nowTick = glfwGetTime();
    starrg.Update(nowTick - lastTick);
    starrg.Render();

    if (!context.Update())
    {
      break;
    }

    if (Mailbox()->Poll(&msg))
    {
      if (bb::As<bb::msg::poison_t>(msg))
      {
        loop = false;
        continue;
      }

      if (auto* defMsg = msg.get())
      {
        bb::Error("Unknown action type: %s", typeid(*defMsg).name());
      }
      else
      {
        bb::Error("Unknown action type: %s", "Invalid Message");
      }
      loop = false;
      BB_PANIC();
    }

    lastTick = nowTick;
  }

  return EXIT_SUCCESS;
}