#pragma once
#ifndef __SUB3000_SCENE_HEADER__
#define __SUB3000_SCENE_HEADER__

#include <memory>
#include <string>

namespace sub3000
{

  enum class sceneID_t
  {
    splash = 0,
    mainMenu,
    authors
  };

  class scene_t
  {
    std::string title;
    sceneID_t sceneID;
    bool isReady;

    virtual void OnPrepare() = 0;
    virtual void OnUpdate(double delta) = 0;
    virtual void OnRender() = 0;
    virtual void OnCleanup() = 0;

  public:

    const std::string& Title() const
    {
      return this->title;
    }

    sceneID_t SceneID() const
    {
      return this->sceneID;
    }

    bool IsReady() const
    {
      return this->isReady;
    }

    void Prepare()
    {
      if (!this->isReady)
      {
        this->OnPrepare();
        this->isReady = true;
      }
    }

    void Update(double delta)
    {
      this->OnUpdate(delta);
    }

    void Render()
    {
      this->OnRender();
    }

    void Cleanup()
    {
      if (this->isReady)
      {
        this->OnCleanup();
        this->isReady = false;
      }
    }

    scene_t(sceneID_t sceneID, const std::string& title);

    virtual ~scene_t() = 0;
  };

  scene_t* GetScene(sceneID_t id);

  scene_t* TopScene(size_t depth);
  void PushScene(scene_t* scene);
  void PopScene();

  void PostChangeScene(sceneID_t newScene);

  void PostExit();

} // namespace sub3000

#endif /* __SUB3000_SCENE_HEADER__ */