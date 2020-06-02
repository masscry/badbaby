#include <common.hpp>
#include <context.hpp>
#include <camera.hpp>
#include <sub3000.hpp>
#include <scene.hpp>
#include <mailbox.hpp>
#include <msg.hpp>
#include <mapGen.hpp>
#include <worker.hpp>
#include <monfs.hpp>

namespace 
{

  bb::mailbox_t::shared_t& Mailbox()
  {
    static bb::mailbox_t::shared_t mail = bb::postOffice_t::Instance().New("sub3000");
    return mail;
  }

  std::mutex g_mapGenLock;
  bb::actorPID_t g_mapGenActorID = -1;

}

namespace sub3000
{

  void PostToMain(bb::msg_t&& msg)
  {
    Mailbox()->Put(std::move(msg));
  }

  bool RequestGenerateMap(bb::actorPID_t sendResultToID)
  {
    std::unique_lock<std::mutex> lock(g_mapGenLock);
    if (g_mapGenActorID == -1)
    {
      return false;
    }

    auto config = bb::config_t("genmap.config");

    auto maxWidth = static_cast<uint16_t>(config.Value("map.width", 2048.0));
    auto maxHeight = static_cast<uint16_t>(config.Value("map.height", 2048.0));

    auto mapSeed = static_cast<int64_t>(config.Value("map.seed", 0.0));
    auto mapRadiusStart = static_cast<float>(config.Value("map.radius.start", 1.0));
    auto mapRadiusFinish = static_cast<float>(config.Value("map.radius.finish", 10.0));
    auto mapRadiusRounds = static_cast<size_t>(config.Value("map.radius.rounds", 10.0));
    auto mapFalloff = static_cast<float>(config.Value("map.falloff", 0.2));
    auto mapPower = static_cast<float>(config.Value("map.power", 2.0));

    auto& pool = bb::workerPool_t::Instance();
    pool.PostMessage(
      g_mapGenActorID,
      bb::Issue<bb::ext::generate_t>(
        sendResultToID,
        maxWidth,
        maxHeight,
        mapRadiusStart,
        mapRadiusFinish,
        mapSeed,
        mapFalloff,
        mapRadiusRounds,
        mapPower
      )
    );
    return true;
  }

}

void ProcessGameAction(bb::actorPID_t src, sub3000::gameAction_t action)
{
  bb::Debug("GameAction: %u from %ld", static_cast<uint32_t>(action), src);
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
  case sub3000::gameAction_t::demo:
    sub3000::PostChangeScene(sub3000::sceneID_t::demo);
    break;
  default:
    assert(0);
  }
}

int fsMonitorIDCounter = 0;
std::unordered_map<int, bb::fs::monitor_t> fsMonitors;

int main(int argc, char* argv[])
{
  if (bb::ProcessStartupArguments(argc, argv) != 0)
  {
    return -1;
  }

  auto& pool = bb::workerPool_t::Instance();
  {
    std::unique_lock<std::mutex> lock(g_mapGenLock);
    g_mapGenActorID = pool.Register<bb::ext::mapGen_t>();
  }

  auto& context = bb::context_t::Instance();

  bb::config_t defaultConfig = bb::config_t("default.config");
  auto sceneName = defaultConfig.Value("scene", "Splash");
  auto sceneID = sub3000::StringToSceneID(sceneName);

  if (sceneID == sub3000::sceneID_t::undef)
  {
    bb::Error("Unknown Scene: \"%s\". Abort!", sceneName.c_str());
    return -1;
  }

  sub3000::PushScene(sub3000::GetScene(sceneID));
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

    if (Mailbox()->Poll(&msgToMain))
    {
      if (auto newWinTitle = bb::As<bb::msg::updateTitle_t>(msgToMain))
      {
        context.Title(newWinTitle->Title());
        continue;
      }

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
        bb::Debug("Action: %s from %ld", sub3000::GetTextForAction(gameAction), action->Source());
        ProcessGameAction(
          action->Source(),
          gameAction
        );
        continue;
      }

      if (auto watch = bb::As<sub3000::fs::watch_t>(msgToMain))
      {
        auto newMon = bb::fs::monitor_t::Create(std::move(watch->Processor()));
        auto result = newMon.Watch(watch->Filename().c_str());

        if (result == -1)
        { // add watch failed, so just return
          if (watch->Source() != bb::INVALID_ACTOR)
          {
            bb::workerPool_t::Instance().PostMessage(
              watch->Source(), 
              bb::Issue<sub3000::fs::status_t>(-1)
            );
          }
          continue;
        }

        // otherwise, store monitor and return it's id
        fsMonitors[fsMonitorIDCounter] = std::move(newMon);

        if (watch->Source() != bb::INVALID_ACTOR)
        {
          bb::workerPool_t::Instance().PostMessage(
            watch->Source(), 
            bb::Issue<sub3000::fs::status_t>(fsMonitorIDCounter)
          );
        }
        ++fsMonitorIDCounter;
        continue;
      }

      if (auto rmWatch = bb::As<sub3000::fs::rmWatch_t>(msgToMain))
      {
        fsMonitors.erase(rmWatch->Watch());
        continue;
      }

      if (auto* defMsg = msgToMain.get())
      {
        bb::Error("Unknown action type: %s", typeid(*defMsg).name());
      }
      else
      {
        bb::Error("Unknown action type: %s", "Invalid Message");
      }
      assert(0);
    }

    for (auto& monitor: fsMonitors)
    {
      monitor.second.Check();
    }

  }

  {
    std::unique_lock<std::mutex> lock(g_mapGenLock);
    pool.Unregister(g_mapGenActorID);
  }

  return 0;
}
