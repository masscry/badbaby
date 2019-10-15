#include <common.hpp>
#include <worker.hpp>

enum simpleMessage_t {
  hello = 0
};

class hello_t: public bb::actor_t
{
  void ProcessMessage(bb::msg_t msg) override
  {
    switch (msg.type)
    {
    case simpleMessage_t::hello:
      bb::Info("Hello!");
      break;
    default:
      bb::Error("Unknown Message Type: %d", msg.type);
      break;
    }
  }
};

int main(int argc, char* argv[])
{
  if (bb::ProcessStartupArguments(argc, argv) != 0)
  {
    return -1;
  }

  bb::workerPool_t::Instance().Register("global.hello", std::unique_ptr<bb::actor_t>(new hello_t));

  for (int i = 0; i < 10; ++i)
  {
    bb::Debug("Say Hello!");
    bb::workerPool_t::Instance().PostMessage("global.hello", bb::msg_t{ simpleMessage_t::hello, nullptr, "" });
    std::this_thread::sleep_for(std::chrono::seconds(2));
  }
  return 0;
}
