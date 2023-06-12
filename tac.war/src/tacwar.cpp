#include <tacwar.hpp>

namespace tac
{

  namespace
  {
    std::mutex sceneCS;
    scene_t* scenes[asInteger(tac::state_t::total)] = { nullptr };
  }

  int scene_t::Register(scene_t* scene)
  {
    if (scene == nullptr)
    {
      return -1;
    }

    if (scene->ID() == state_t::undef)
    {
      return -1;
    }

    auto index = asInteger(scene->ID());
    std::unique_lock<std::mutex> lock(sceneCS);
    if (scenes[index] != nullptr)
    {
      return -1;
    }

    scenes[index] = scene;
    return 0;
  }

  int scene_t::Deregister(scene_t* scene)
  {
    if (scene == nullptr)
    {
      return -1;
    }

    if (scene->ID() == state_t::undef)
    {
      return -1;
    }    

    auto index = asInteger(scene->ID());
    std::unique_lock<std::mutex> lock(sceneCS);
    if (scenes[index] == nullptr)
    {
      return -1;
    }

    if (scenes[index] != scene)
    {
      return -1;
    }

    scenes[index] = nullptr;
    return 0;
  }

  int scene_t::SceneAction(state_t state, int action)
  {
    std::unique_lock<std::mutex> lock(sceneCS);
    if (state == state_t::undef)
    {
      return -1;
    }

    auto scene = scenes[asInteger(state)];
    if (scene == nullptr)
    {
      return -1;
    }

    scene->OnAction(action);
    return 0;
  }

  int scene_t::SceneMouse(state_t state, int btn, int press)
  {
    std::unique_lock<std::mutex> lock(sceneCS);
    if (state == state_t::undef)
    {
      return -1;
    }

    auto scene = scenes[asInteger(state)];
    if (scene == nullptr)
    {
      return -1;
    }

    scene->OnMouse(btn, press);
    return 0;
  }

  int scene_t::SceneClick(state_t state)
  {
    std::unique_lock<std::mutex> lock(sceneCS);
    if (state == state_t::undef)
    {
      return -1;
    }

    auto scene = scenes[asInteger(state)];
    if (scene == nullptr)
    {
      return -1;
    }

    scene->OnClick();
    return 0;
  }

  int scene_t::SceneUpdate(state_t state, double dt)
  {
    std::unique_lock<std::mutex> lock(sceneCS);
    if (state == state_t::undef)
    {
      return -1;
    }

    auto scene = scenes[asInteger(state)];
    if (scene == nullptr)
    {
      return -1;
    }

    scene->OnUpdate(dt);
    scene->OnRender();
    return 0;
  }

  void scene_t::OnMouse(int, int)
  {
    // Do nothing!
    ;
  }

}

namespace
{
  bb::mailbox_t::shared_t& Mailbox()
  {
    static auto mail = bb::postOffice_t::Instance().New("tacwar");
    return mail;
  }
}

int main(int argc, char* argv[])
{
  if (bb::ProcessStartupArguments(argc, argv) != 0)
  {
    return EXIT_FAILURE;
  }

  auto& context = bb::context_t::Instance();
  bb::workerPool_t::Instance();
  tac::menu_t menu;
  tac::game_t game;

  context.RegisterMailboxCallback(
    "tacwar",
    bb::context_t::msgFlag_t::mouse
  );

  tac::state_t state = tac::state_t::game;
  bb::msg_t msg;
  auto lastTick = glfwGetTime();

  while (context.Update())
  {
    auto nowTick = glfwGetTime();
    if (tac::scene_t::SceneUpdate(state, nowTick - lastTick) != 0)
    {
      bb::Error("SceneUpdate failed! (%d)\n", tac::asInteger(state));
    }

    if (Mailbox()->Poll(&msg))
    {
      if (bb::As<bb::msg::poison_t>(msg))
      {
        break;
      }
      if (bb::As<bb::msg::clickEvent_t>(msg))
      {
        tac::scene_t::SceneClick(state);
        continue;
      }
      if (auto mouse = bb::As<bb::msg::mouseEvent_t>(msg))
      {
        tac::scene_t::SceneMouse(state, mouse->Key(), mouse->Press());
        continue;
      }
      if (auto action = bb::As<bb::msg::dataMsg_t<int>>(msg))
      {
        tac::scene_t::SceneAction(state, action->Data());
        continue;
      }
      if (auto changeScene = bb::As<bb::msg::dataMsg_t<tac::state_t>>(msg))
      {
        state = changeScene->Data();
        continue;
      }

      if (auto* defMsg = msg.get())
      {
        bb::Error("Unknown action type: %s", typeid(*defMsg).name());
      }
      else
      {
        bb::Error("Unknown action type: %s", "Invalid Message");
      }
      BB_PANIC();
      break;
    }

    lastTick = nowTick;
  }

  return EXIT_SUCCESS;
}