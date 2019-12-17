#include <common.hpp>
#include <context.hpp>
#include <camera.hpp>
#include <sub3000.hpp>
#include <scene.hpp>
#include <mailbox.hpp>
#include <msg.hpp>

namespace 
{

  bb::mailbox_t mail;

}

namespace sub3000
{

  void PostToMain(bb::msg_t msg)
  {
    mail.Put(msg);
  }

}


int main(int argc, char* argv[])
{
  if (bb::ProcessStartupArguments(argc, argv) != 0)
  {
    return -1;
  }

  auto& context = bb::context_t::Instance();

  sub3000::PushScene(sub3000::GetScene(sub3000::sceneID_t::splash));
  sub3000::deltaTime_t dt;
  bb::msg_t msgToMain;

  while(true)
  {
    auto topScene = sub3000::TopScene(0);
    topScene->Update(dt.Mark());
    topScene->Render();

    if (!context.Update())
    {
      break;
    }

    if (mail.Poll(&msgToMain))
    {
      switch(msgToMain.type)
      {
        case sub3000::nop:
          break;
        case sub3000::change_scene:
          sub3000::PopScene();
          sub3000::PushScene(sub3000::GetScene(
            bb::GetMsgData<sub3000::sceneID_t>(msgToMain)
          ));
          break;
        default:
          assert(0);
      }
    }
  }

  return 0;
}
