#pragma once
#ifndef __SUB3000_SCENE_HEADER__
#define __SUB3000_SCENE_HEADER__

#include <memory>

namespace sub3000
{

  class scene_t
  {
  public:

    virtual void OnPrepare() = 0;
    virtual void OnUpdate(double delta) = 0;
    virtual void OnRender() = 0;
    virtual void OnCleanup() = 0;

    virtual ~scene_t() = 0;
  };

  enum class sceneList_t
  {
    splash = 0
  };

  using uniqueScene_t = std::unique_ptr<scene_t>;

  uniqueScene_t GetScene(sceneList_t id);

} // namespace sub3000

#endif /* __SUB3000_SCENE_HEADER__ */