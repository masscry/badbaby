/**
 * @file msg.hpp
 * 
 * Simple message struct
 * 
 */
#pragma once
#ifndef __BB_CORE_MSG_HEADER__
#define __BB_CORE_MSG_HEADER__

#include <cstring>

namespace bb
{

  struct msg_t final
  {
    int     src;
    int     type;
    uint8_t data[8];
  };

  template<typename data_t>
  msg_t MakeMsg(int src, int type, data_t data)
  {
    msg_t result;
    static_assert(sizeof(data_t) <= sizeof(msg_t::data));
    static_assert(std::is_pod<data_t>::value);

    result.src = src;
    result.type = type;
    memcpy(result.data, &data, sizeof(data_t));
    return result;
  }

  template<typename data_t>
  data_t GetMsgData(const msg_t& msg)
  {
    static_assert(sizeof(data_t) <= sizeof(msg_t::data));
    static_assert(std::is_pod<data_t>::value);

    data_t result;
    memcpy(&result, msg.data, sizeof(data_t));
    return result;
  }

}

#endif /* __BB_CORE_MSG_HEADER__ */
