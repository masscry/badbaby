#include <scene.hpp>
#include <splash.hpp>
#include <mainMenu.hpp>
#include <deque>
#include <set>
#include <cassert>

#include <msg.hpp>
#include <sub3000.hpp>

namespace
{
  using sceneStack_t = std::deque<sub3000::scene_t*>;
  sceneStack_t sceneStack;

  using sceneSet_t = std::set<sub3000::sceneID_t>;
  sceneSet_t sceneSet;

}

namespace sub3000
{

  scene_t::scene_t(sceneID_t sceneID, const std::string& title)
  : title(title),
    sceneID(sceneID),
    isReady(false)
  {
    ;
  }

  scene_t::~scene_t()
  {
    ;
  }

  scene_t* GetScene(sceneID_t id)
  {
    switch (id)
    {
    case sceneID_t::splash:
      {
        static splashScene_t splash;
        return &splash;
      }
    case sceneID_t::mainMenu:
      {
        static mainMenuScene_t mainMenu;
        return &mainMenu;
      }
    default:
      throw std::runtime_error("Unknown Scene Requested");
    }
  }

  scene_t* TopScene(size_t depth)
  {
    assert(depth < sceneStack.size());
    return *(sceneStack.rbegin() + depth);
  }

  void PushScene(scene_t* scene)
  {
    assert(scene != nullptr);
    sceneSet.insert(scene->SceneID());
    sceneStack.push_back(scene);
    scene->Prepare();
    bb::context_t::Instance().Title(scene->Title());
  }

  void PopScene()
  {
    assert(!sceneStack.empty());
    sceneSet.erase(TopScene(0)->SceneID());
    TopScene(0)->Cleanup();
    sceneStack.pop_back();
  }

  void PostChangeScene(sceneID_t newScene)
  {
    PostToMain(bb::MakeMsg(0, static_cast<int>(mainMessage_t::change_scene), newScene));
  }

} // namespace sub3000