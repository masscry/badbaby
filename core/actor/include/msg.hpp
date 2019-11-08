/**
 * @file msg.hpp
 * 
 * Simple message struct
 * 
 */
#pragma once
#ifndef __BB_CORE_MSG_HEADER__
#define __BB_CORE_MSG_HEADER__

namespace bb
{

  struct msg_t final
  {
    int   src;
    int   type;
    void* data;
  };

}

#endif /* __BB_CORE_MSG_HEADER__ */
