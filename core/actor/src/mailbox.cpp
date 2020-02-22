#include <mailbox.hpp>

#include <cassert>

namespace bb
{

  size_t mailbox_t::Has()
  {
    std::unique_lock<std::mutex> lock(this->guard);
    return this->storage.size();
  }

  bool mailbox_t::Empty()
  {
    std::unique_lock<std::mutex> lock(this->guard);
    return this->storage.empty();
  }

  bool mailbox_t::Poll(msg_t* result)
  {
    assert(result != nullptr);

    std::unique_lock<std::mutex> lock(this->guard);
    if (this->storage.empty())
    {
      return false;
    }

    *result = std::move(this->storage.front());
    this->storage.pop();
    return true;
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

  void mailbox_t::Put(msg_t&& msg)
  {
    std::lock_guard<std::mutex> lock(this->guard);
    this->storage.emplace(std::move(msg));
    this->notify.notify_one();
  }

} // namespace bb