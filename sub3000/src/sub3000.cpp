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

void ProcessGameAction(sub3000::gameAction_t action)
{
  switch (action)
  {
  case sub3000::gameAction_t::newGame:
    sub3000::PostChangeScene(sub3000::sceneID_t::splash);
    break;
  case sub3000::gameAction_t::loadGame:
    break;
  case sub3000::gameAction_t::authors:
    break;
  case sub3000::gameAction_t::exit:
    sub3000::PostExit();
    break;
  case sub3000::gameAction_t::settings:
    break;
  default:
    assert(0);
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

  bool loop = true;
  while(loop)
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
        case sub3000::exit:
          sub3000::PopScene();
          loop = false;
          break;
        case sub3000::action:
          {
            auto gameAction = bb::GetMsgData<sub3000::gameAction_t>(msgToMain);
            bb::Debug("Action: %s", sub3000::GetTextForAction(gameAction));
            ProcessGameAction(gameAction);
          }
          break;
        default:
          assert(0);
      }
    }
  }

  return 0;
}
