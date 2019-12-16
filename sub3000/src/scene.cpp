#include <scene.hpp>
#include <splash.hpp>

namespace sub3000
{

  scene_t::~scene_t()
  {
    ;
  }

  uniqueScene_t GetScene(sceneList_t id)
  {
    switch (id)
    {
    case sceneList_t::splash:
      return uniqueScene_t(new splashScene_t);
    default:
      throw std::runtime_error("Unknown Scene Requested");
    }
  }

} // namespace sub3000
