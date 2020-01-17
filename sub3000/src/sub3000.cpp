#include <common.hpp>
#include <context.hpp>
#include <camera.hpp>
#include <sub3000.hpp>
#include <scene.hpp>
#include <mailbox.hpp>
#include <msg.hpp>
#include <mapGen.hpp>
#include <worker.hpp>

namespace 
{

  bb::mailbox_t mail;

  std::mutex g_mapGenLock;
  int g_mapGenActorID = -1;

}

namespace sub3000
{

  void PostToMain(bb::msg_t msg)
  {
    mail.Put(msg);
  }

  bool RequestGenerateMap(uint16_t width, uint16_t height, float radius, int sendResultToID)
  {
    std::unique_lock<std::mutex> lock(g_mapGenLock);
    if (g_mapGenActorID == -1)
    {
      return false;
    }

    auto& pool = bb::workerPool_t::Instance();
    pool.PostMessage(
      g_mapGenActorID,
      bb::MakeMsg(sendResultToID, sub3000::mapGenMsg_t::generate, 
        sub3000::mapGenerateParams_t {
          width, height, radius
        }
      )
    );
    return true;
  }

}

void ProcessGameAction(int src, sub3000::gameAction_t action)
{
  switch (action)
  {
  case sub3000::gameAction_t::newGame:
    bb::Debug("%s", "Map Generation Started");
    sub3000::RequestGenerateMap(2048, 1024, 10.0f, src);
    break;
  case sub3000::gameAction_t::logo:
    sub3000::PostChangeScene(sub3000::sceneID_t::splash);
    break;
  case sub3000::gameAction_t::authors:
    sub3000::PostChangeScene(sub3000::sceneID_t::authors);
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

  auto& pool = bb::workerPool_t::Instance();
  {
    std::unique_lock<std::mutex> lock(g_mapGenLock);
    g_mapGenActorID = pool.Register(std::unique_ptr<bb::role_t>(new sub3000::mapGen_t()));
  }

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
            bb::Debug("Action: %s from %d", sub3000::GetTextForAction(gameAction), msgToMain.src);
            ProcessGameAction(msgToMain.src, gameAction);
          }
          break;
        default:
          assert(0);
      }
    }
  }

  {
    std::unique_lock<std::mutex> lock(g_mapGenLock);
    pool.Unregister(g_mapGenActorID);
  }

  return 0;
}
