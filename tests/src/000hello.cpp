#include <common.hpp>
#include <worker.hpp>
#include <role.hpp>
#include <msg.hpp>

using namespace bb;

enum simpleMessage_t: uint16_t
{
  hello = bb::msgID_t::USR00
};

class hello_t: public role_t
{
  msgResult_t OnProcessMessage(const actor_t&, msg_t msg) override
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
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return msgResult_t::complete;
  }

  const char* DefaultName() const override
  {
    return "hello_t";
  }

};

int main(int argc, char* argv[])
{
  if (ProcessStartupArguments(argc, argv) != 0)
  {
    return -1;
  }

  int actorID = workerPool_t::Instance().Register(std::unique_ptr<role_t>(new hello_t));

  for (int i = 0; i < 20; ++i)
  {
    Debug("Say Hello!");
    workerPool_t::Instance().PostMessage(actorID, MakeMsg(-1, simpleMessage_t::hello, 0));
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  workerPool_t::Instance().Unregister(actorID);
  return 0;
}
