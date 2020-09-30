#pragma once
#ifndef TACWAR_HEADER
#define TACWAR_HEADER

#include <common.hpp>
#include <context.hpp>
#include <mailbox.hpp>
#include <shader.hpp>
#include <camera.hpp>
#include <shapes.hpp>
#include <image.hpp>
#include <worker.hpp>
#include <font.hpp>
#include <text.hpp>
#include <role.hpp>

namespace tac
{

  template <typename enum_t>
  constexpr typename std::underlying_type<enum_t>::type asInteger(enum_t eitem) noexcept 
  {
    return static_cast<typename std::underlying_type<enum_t>::type>(eitem);
  }

  enum class state_t: int32_t
  {
    undef = -1,
    menu = 0,
    game,
    total
  };

  class scene_t
  {
    tac::state_t id;

    virtual void OnClick() = 0;
    virtual void OnUpdate(double dt) = 0;
    virtual void OnAction(int action) = 0;
    virtual void OnRender() = 0;

    static int Register(scene_t* scene);
    static int Deregister(scene_t* scene);

  public:

    scene_t(const scene_t&) = delete;
    scene_t& operator =(const scene_t&) = delete;
    scene_t(scene_t&&) = delete;
    scene_t& operator =(scene_t&&) = delete;

    tac::state_t ID() const
    {
      return this->id;
    }

    scene_t(tac::state_t id)
    : id(id)
    {
      if (scene_t::Register(this) != 0)
      {
        throw std::runtime_error("Register Failed");
      }
    }

    ~scene_t()
    {
      if (scene_t::Deregister(this) != 0)
      {
        bb::Error("Deregister failed for %u (%s)", asInteger(this->id), typeid(*this).name());
      }
    }

    static int SceneUpdate(state_t state, double dt);
    static int SceneClick(state_t state);
    static int SceneAction(state_t state, int action);
  };

} // namespace tac

#include "tacmenu.hpp"
#include "tacgame.hpp"

#endif /* TACWAR_HEADER */