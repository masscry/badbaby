#include <common.hpp>
#include <worker.hpp>

using namespace bb;

enum simpleMessage_t {
  hello = 0
};

class hello_t: public actor_t
{
  void ProcessMessage(msg_t msg) override
  {
    switch (msg.type)
    {
    case simpleMessage_t::hello:
      Info("Hello!");
      break;
    default:
      Error("Unknown Message Type: %d", msg.type);
      break;
    }
  }
};

int main(int argc, char* argv[])
{
  if (ProcessStartupArguments(argc, argv) != 0)
  {
    return -1;
  }

  workerPool_t::Instance().Register("global.hello", std::unique_ptr<actor_t>(new hello_t));

  for (int i = 0; i < 10; ++i)
  {
    workerPool_t::Instance().PostMessage("global.hello", msg_t{ simpleMessage_t::hello, nullptr });
    std::this_thread::sleep_for(std::chrono::seconds(2));
  }
  return 0;
}
