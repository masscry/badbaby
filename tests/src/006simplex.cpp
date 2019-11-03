#include <common.hpp>
#include <context.hpp>
#include <camera.hpp>
#include <actor.hpp>
#include <worker.hpp>

int main(int argc, char* argv[])
{
  if (bb::ProcessStartupArguments(argc, argv) != 0)
  {
    return -1;
  }

  auto& context = bb::context_t::Instance();

  for(;;)
  {
    if (!context.Update())
    {
      break;
    }
  }

  return 0;
}