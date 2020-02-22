/**
 * @file mailbox.hpp
 * 
 * Message dispatching system
 * 
 */

#pragma once
#ifndef __BB_CORE_MAILBOX_HEADER__
#define __BB_CORE_MAILBOX_HEADER__

#include <queue>
#include <mutex>
#include <condition_variable>

#include <msg.hpp>

namespace bb
{

  class mailbox_t final
  {
    using storage_t = std::queue<msg_t>;

    std::mutex guard;
    std::condition_variable notify;
    storage_t  storage;

    mailbox_t(const mailbox_t&) = delete;
    mailbox_t(mailbox_t&&) = delete;

    mailbox_t& operator=(const mailbox_t&) = delete;
    mailbox_t& operator=(mailbox_t&&) = delete;

  public:

    size_t Has();

    bool Empty();

    msg_t Wait();

    bool Poll(msg_t* result);

    void Put(msg_t&& msg);

    mailbox_t() = default;
    ~mailbox_t() = default;

  };

}

#endif /* __BB_CORE_MAILBOX_HEADER__ */