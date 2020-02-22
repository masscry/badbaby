#include <space.hpp>
#include <simplex.hpp>
#include <worker.hpp>
#include <msg.hpp>

#include <cassert>
#include <cmath>

#include <glm/gtc/constants.hpp>
#include <glm/vec3.hpp>


namespace sub3000
{
  bb::msg::result_t space_t::OnProcessMessage(const bb::actor_t&, const bb::msg::basic_t& msg)
  {
    if (bb::msg::As<step_t>(msg) != nullptr)
    {
      return bb::msg::result_t::complete;
    }

    bb::Error("Unknown message: %s", typeid(msg).name());
    assert(0);
    return bb::msg::result_t::error;
  }

  space_t::space_t()
  {
    ;
  }

}