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

  bb::mailbox_t::shared_t mail = bb::postOffice_t::Instance().New("sub3000");

  std::mutex g_mapGenLock;
  int g_mapGenActorID = -1;

}

namespace sub3000
{

  void PostToMain(bb::msg_t&& msg)
  {
    mail->Put(std::move(msg));
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
      bb::Issue<sub3000::generate_t>(
        sendResultToID,
        width, height, radius
      )
    );
    return true;
  }

}

void ProcessGameAction(int src, sub3000::gameAction_t action)
{
  bb::Debug("GameAction: %u from %d", static_cast<uint32_t>(action), src);
  switch (action)
  {
  case sub3000::gameAction_t::newGame:
    sub3000::PostChangeScene(sub3000::sceneID_t::arena);
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

    if (mail->Poll(&msgToMain))
    {
      if (auto changeScene = bb::As<sub3000::changeScene_t>(msgToMain))
      {
        sub3000::PopScene();
        sub3000::PushScene(
          sub3000::GetScene(
            changeScene->SceneID()
          )
        );
        continue;
      }

      if (bb::As<sub3000::exit_t>(msgToMain))
      {
        sub3000::PopScene();
        loop = false;
        continue;
      }

      if (auto action = bb::As<sub3000::action_t>(msgToMain))
      {
        auto gameAction = action->GameAction();
        bb::Debug("Action: %s from %d", sub3000::GetTextForAction(gameAction), action->Source());
        ProcessGameAction(
          action->Source(),
          gameAction
        );
        continue;
      }

      bb::Error("Unknown action type: %s", typeid(*msgToMain.get()).name());
      assert(0);
    }
  }

  {
    std::unique_lock<std::mutex> lock(g_mapGenLock);
    pool.Unregister(g_mapGenActorID);
  }

  return 0;
}
