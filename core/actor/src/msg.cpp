#include <msg.hpp>

namespace bb
{

  namespace msg
  {
    basic_t::~basic_t()
    {
      ;
    }

  } // namespace msg

  msg_t IssuePoison()
  {
    return msg_t(new bb::msg::poison_t);
  }

  msg_t IssueSetName(const char* name)
  {
    return msg_t(new bb::msg::setName_t(name));
  }

} // namespace bb
