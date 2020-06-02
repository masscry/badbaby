#include <mailbox.hpp>
#include <common.hpp>

#include <cassert>
#include <functional>

namespace bb
{

  bool mailbox_t::Poll(msg_t* result)
  {
    assert(result != nullptr);

    std::lock_guard<std::mutex> lock(this->guard);
    if (this->storage.empty())
    {
      return false;
    }

    *result = std::move(this->storage.front());
    this->storage.pop();
    return true;
  }

  void mailbox_t::Put(msg_t&& msg)
  {
    std::lock_guard<std::mutex> lock(this->guard);
    this->storage.emplace(std::move(msg));
    this->notify.notify_one();
  }

  msg_t mailbox_t::Wait()
  {
    std::unique_lock<std::mutex> lock(this->guard);
    if (!this->storage.empty())
    {
      msg_t result = std::move(this->storage.front());
      this->storage.pop();
      return result;
    }

    this->notify.wait(lock, [this](){ return !this->storage.empty(); });
    msg_t result = std::move(this->storage.front());
    this->storage.pop();
    return result;
  }

  postOffice_t& postOffice_t::Instance()
  {
    static postOffice_t postOffice;
    return postOffice;
  }

  mailbox_t::shared_t postOffice_t::New(postAddress_t address)
  {
    std::lock_guard<std::mutex> lock(this->guard);

    auto item = this->storage.find(address);
    if (item != this->storage.end())
    {
      bb::Error("Item with address %u already exists!", address);
      // such item exists
      assert(0);
      return mailbox_t::shared_t();
    }

    auto result = std::make_shared<mailbox_t>(address);
    this->storage[address] = result;
    return result;
  }

  void postOffice_t::Delete(postAddress_t address)
  {
    std::lock_guard<std::mutex> lock(this->guard);
    auto postBox = this->storage.find(address);
    if (postBox != this->storage.end())
    {
      this->storage.erase(postBox);
    }
  }

  mailbox_t::shared_t postOffice_t::New(const std::string& address)
  {
    return this->New(static_cast<postAddress_t>(std::hash<std::string>()(address)));
  }

  mailbox_t::mailbox_t(postAddress_t address)
  : address(address)
  {
    ;
  }

  mailbox_t::~mailbox_t()
  {
    postOffice_t::Instance().Delete(this->Address());
  }

  int postOffice_t::Post(const std::string& address, msg_t&& msg)
  {
    return this->Post(static_cast<postAddress_t>(std::hash<std::string>()(address)), std::move(msg));
  }

  int postOffice_t::Post(postAddress_t address, msg_t&& msg)
  {
    std::lock_guard<std::mutex> lock(this->guard);
    auto item = this->storage.find(address);
    if (item == this->storage.end())
    {
      return -1;
    }

    auto mailbox = item->second.lock();
    if (!mailbox)
    {
      return -1;
    }

    mailbox->Put(std::move(msg));
    return 0;
  }

  postOffice_t::postOffice_t()
  {
    bb::Debug("%s", "Post Office Created");
  }

  postOffice_t::~postOffice_t()
  {
    bb::Debug("%s", "Post Office Deleted");
  }


} // namespace bb