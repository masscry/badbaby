/**
 * @file msg.hpp
 * 
 * Simple message struct
 * 
 */
#pragma once
#ifndef __BB_CORE_MSG_HEADER__
#define __BB_CORE_MSG_HEADER__

#include <memory>

namespace bb
{

  class actor_t;

  using msgData_t   = std::shared_ptr<void*>;

  struct msg_t final
  {
    int         type;
    msgData_t   data;
  };

}

#endif /* __BB_CORE_MSG_HEADER__ */
