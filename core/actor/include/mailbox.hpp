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
#include <unordered_map>
#include <memory>

#include <msg.hpp>

namespace bb
{
  
  using postAddress_t = uint32_t;

  class mailbox_t final
  {
    friend class postOffice_t;

    using storage_t = std::queue<msg_t>;

    mutable std::mutex guard;
    mutable std::condition_variable notify;
    storage_t storage;
    postAddress_t address;

  public:

    explicit mailbox_t(postAddress_t address);
    ~mailbox_t();

    mailbox_t(const mailbox_t&) = delete;
    mailbox_t(mailbox_t&&) = delete;

    mailbox_t& operator=(const mailbox_t&) = delete;
    mailbox_t& operator=(mailbox_t&&) = delete;

    using shared_t = std::shared_ptr<mailbox_t>;
    using weak_t = std::weak_ptr<mailbox_t>;

    postAddress_t Address() const;

    size_t Has() const;

    bool Empty() const;

    msg_t Wait();

    bool Poll(msg_t* result);

    void Put(msg_t&& msg);

  };

  postAddress_t GetPostAddressFromString(const std::string& str);

  class postOffice_t final
  {
    friend class mailbox_t;

    using storage_t = std::unordered_map<postAddress_t, mailbox_t::weak_t>;

    std::mutex guard;
    storage_t storage;

    postOffice_t();
    ~postOffice_t();

    postOffice_t(const postOffice_t&) = delete;
    postOffice_t& operator=(const postOffice_t&) = delete;
    postOffice_t(postOffice_t&&) = delete;
    postOffice_t& operator=(postOffice_t&&) = delete;

    void Delete(postAddress_t address);

    mailbox_t::shared_t New(postAddress_t address);

  public:

    static postOffice_t& Instance();

    mailbox_t::shared_t New(const std::string& address);
    int Post(const std::string& address, msg_t&& msg);
    int Post(postAddress_t address, msg_t&& msg);
  };

  inline postAddress_t mailbox_t::Address() const
  {
    return this->address;
  }

  inline size_t mailbox_t::Has() const
  {
    std::unique_lock<std::mutex> lock(this->guard);
    return this->storage.size();
  }

  inline bool mailbox_t::Empty() const
  {
    std::unique_lock<std::mutex> lock(this->guard);
    return this->storage.empty();
  }

}

#endif /* __BB_CORE_MAILBOX_HEADER__ */