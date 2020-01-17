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
#include <cstdint>

#include <type_traits>

namespace bb
{

  enum msgID_t: uint16_t
  {
    NOOP     = 0x0000,
    POISON   = 0x0001,
    SETNAME  = 0x0002,
    SETID    = 0x0003,
    KEYBOARD = 0x0004,
    LASTSYS  = 0x00FF,
    USR00    = 0x0100,
    TOTAL    = 0xFFFF
  };

  enum class msgResult_t
  {
    skipped = 0,
    complete = 1,
    error,
    poisoned
  };

  using actorPID_t = int16_t;

  const size_t msgDataByteLength = 8;

  struct msg_t final
  {
    actorPID_t src;
    uint16_t   type;
    uint8_t    data[msgDataByteLength];
  };

  msg_t IssuePoison();

  msg_t IssueSetID(int id);

  msg_t IssueSetName(const char* name);

  template<typename data_t, typename msgXID_t>
  msg_t MakeMsg(int src, msgXID_t type, data_t data)
  {
    msg_t result;
    static_assert(sizeof(data_t) <= sizeof(msg_t::data));
    static_assert(std::is_pod<data_t>::value);
    static_assert(sizeof(msgXID_t) == sizeof(msgID_t));

    result.src = src;
    result.type = static_cast<uint16_t>(type);
    memcpy(result.data, &data, sizeof(data_t));
    return result;
  }

  template<typename data_t, typename msgXID_t>
  msg_t MakeMsgPtr(int src, msgXID_t type, data_t* pData)
  {
    msg_t result;
    static_assert(sizeof(data_t*) <= sizeof(msg_t::data));
    static_assert(sizeof(msgXID_t) == sizeof(msgID_t));

    result.src = src;
    result.type = static_cast<uint16_t>(type);
    memcpy(result.data, &pData, sizeof(data_t*));
    return result;
  }

  template<typename data_t>
  data_t* GetMsgPtr(const msg_t& msg)
  {
    data_t* result;
    memcpy(&result, msg.data, sizeof(data_t*));
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
