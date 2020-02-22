/**
 * @file actor.hpp
 * 
 * Class implements abstract actor which can receive and send messages.
 * 
 */
#pragma once
#ifndef __BB_CORE_ACTOR_HEADER__
#define __BB_CORE_ACTOR_HEADER__

#include <common.hpp>
#include <msg.hpp>
#include <mailbox.hpp>

namespace bb
{

  class role_t;

  class actor_t final
  {
    mailbox_t               mailbox;
    std::unique_ptr<role_t> role;
    std::mutex              inProcess;
    std::string             name;
    int                     id;
    bool                    sick;

    // role can't be copied
    actor_t(const actor_t&) = delete;
    actor_t& operator=(const actor_t&) = delete;

    // role can't be moved
    actor_t(actor_t&&) = delete;
    actor_t& operator=(actor_t&&) = delete;

    msg::result_t ProcessMessagesCore();

  public:

    bool Sick() const;

    const std::string& Name() const;

    int ID() const;

    msg::result_t ProcessMessages();

    msg::result_t ProcessMessagesReadReleaseAquire(rwMutex_t& mutex);

    void PostMessage(msg_t&& msg);

    bool NeedProcessing();

    actor_t(std::unique_ptr<role_t>&& role);
    ~actor_t();

  };

  inline bool actor_t::Sick() const
  {
    return this->sick;
  }

  inline const std::string& actor_t::Name() const
  {
    return this->name;
  }

  inline int actor_t::ID() const
  {
    return this->id;
  }

  inline void actor_t::PostMessage(msg_t&& msg)
  {
    this->mailbox.Put(std::move(msg));
  }

}

#endif /* __BB_CORE_ACTOR_HEADER__ */