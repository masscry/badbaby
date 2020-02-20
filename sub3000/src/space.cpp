#include <space.hpp>
#include <simplex.hpp>
#include <worker.hpp>

#include <cassert>
#include <cmath>

#include <glm/gtc/constants.hpp>
#include <glm/vec3.hpp>

namespace sub3000
{
  bb::msgResult_t space_t::OnProcessMessage(const bb::actor_t&, bb::msg_t msg)
  {
    switch (static_cast<spaceMsg_t>(msg.type))
    {
      case spaceMsg_t::step:


        break;
      default:
        assert(0);
    }
    return bb::msgResult_t::complete;
  }

  space_t::space_t()
  {
    ;
  }

}