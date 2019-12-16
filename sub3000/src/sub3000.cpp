#include <common.hpp>
#include <context.hpp>
#include <camera.hpp>
#include <sub3000.hpp>
#include <scene.hpp>

int main(int argc, char* argv[])
{
  if (bb::ProcessStartupArguments(argc, argv) != 0)
  {
    return -1;
  }

  auto& context = bb::context_t::Instance();

  auto splash = sub3000::GetScene(sub3000::sceneList_t::splash);
  splash->OnPrepare();

  sub3000::deltaTime_t dt;

  while(true)
  {
    splash->OnUpdate(dt.Mark());
    splash->OnRender();

    if (!context.Update())
    {
      break;
    }
  }

  return 0;
}
