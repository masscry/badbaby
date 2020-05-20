#include <common.hpp>
#include <worker.hpp>
#include <role.hpp>
#include <msg.hpp>

using namespace bb;

class helloMsg_t final: public msg::basic_t
{
public:
  helloMsg_t() { ; }
  ~helloMsg_t() override = default;
};

class hello_t: public role_t
{
  msg::result_t OnProcessMessage(const actor_t&, const msg::basic_t& msg) override
  {
    if (As<helloMsg_t>(msg) != nullptr)
    {
      Info("Hello!");
    }
    else
    {
      Error("Unknown Message Type: %s", typeid(msg).name());
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return msg::result_t::complete;
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

  auto actorID = workerPool_t::Instance().Register<hello_t>();

  for (int i = 0; i < 5; ++i)
  {
    Debug("Say Hello!");
    workerPool_t::Instance().PostMessage(
      actorID,
      Issue<helloMsg_t>()
    );
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  workerPool_t::Instance().Unregister(actorID);
  return 0;
}
