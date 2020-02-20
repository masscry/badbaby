/**
 * @file space.hpp
 * 
 * Tactical world simulation actor
 * 
 */

#pragma once
#ifndef __SUB3000_SPACE_HEADER__
#define __SUB3000_SPACE_HEADER__

#include <actor.hpp>
#include <role.hpp>

namespace sub3000
{

  enum class spaceMsg_t: uint16_t
  {
    firstItem = bb::msgID_t::USR00,
    step = firstItem,
    totalItems
  };

  class space_t final: public bb::role_t
  {

    bb::msgResult_t OnProcessMessage(const bb::actor_t&, bb::msg_t msg) override;

  public:

    const char* DefaultName() const override;

    space_t();
    ~space_t() override = default;

  };

  const char* space_t::DefaultName() const
  {
    return "space";
  }

} // namespace sub3000

#endif /* __SUB3000_SPACE_HEADER__ */