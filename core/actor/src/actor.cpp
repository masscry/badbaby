#include <actor.hpp>
#include <worker.hpp>
#include <role.hpp>

#include <context.hpp>

#include <cassert>

/**
 * TODO: This code needs to have better actor ID management!
 * 
 * When actor died and replaced with another, it may still have to 
 * receive answers, when it does - need to manage them somehow
 * 
 */

namespace bb
{

  msg::result_t actor_t::ProcessMessagesCore()
  {
    if (this->sick)
    {
      return msg::result_t::skipped;
    }

    auto& curRole = *this->role;

    size_t msgAlreadyInQueue = this->mailbox.Has();
    if (msgAlreadyInQueue == 0)
    {
      return msg::result_t::skipped;
    }

    Debug("Process \"%s\" (%08x)", this->Name().c_str(), this->ID());

    auto result = msg::result_t::complete;
    while(msgAlreadyInQueue-->0)
    {
      auto msg = this->mailbox.Wait();

      if (bb::As<bb::msg::poison_t>(msg) != nullptr)
      {
        bb::Debug("Actor \"%s\" (%08x) is poisoned", this->Name().c_str(), this->ID());
        context_t::UnregisterActorCallbacksIfContextExists(this->ID());
        this->sick = true;
        this->id = -1;
        return msg::result_t::poisoned;
      }

      if (auto data = bb::As<bb::msg::setName_t>(msg))
      {
        this->name = data->Name();
        continue; 
      }

      if (auto data = bb::As<bb::msg::setID_t>(msg))
      {
        this->id = data->ID();
        continue;
      }

      auto tmpResult = curRole.ProcessMessage(*this, *msg.get());
      if (tmpResult == msg::result_t::poisoned)
      {
        bb::Debug("Actor \"%s\" (%08x) poisoned himself", this->Name().c_str(), this->ID());
        this->sick = true;
        this->id = -1;
        return msg::result_t::poisoned;
      }
      
      if (tmpResult != msg::result_t::complete)
      {
        result = tmpResult;
      }
    }
    return result;
  }

  msg::result_t actor_t::ProcessMessages()
  {
    std::unique_lock<std::mutex> inProcessLock(this->inProcess, std::try_to_lock);
    if (!inProcessLock.owns_lock())
    {
      return msg::result_t::skipped;
    }
    return this->ProcessMessagesCore();
  }

  msg::result_t actor_t::ProcessMessagesReadReleaseAquire(rwMutex_t& mutex)
  {
    std::unique_lock<std::mutex> inProcessLock(this->inProcess, std::try_to_lock);
    if (!inProcessLock.owns_lock())
    {
      return msg::result_t::skipped;
    }
    mutex.UnlockRead();
    BB_DEFER(mutex.LockRead());
    return this->ProcessMessagesCore();
  }

  bool actor_t::NeedProcessing()
  {
    std::unique_lock<std::mutex> inProcessLock(this->inProcess, std::try_to_lock);
    if (inProcessLock.owns_lock())
    {
      return (!this->mailbox.Empty()) && (this->sick == false);
    }
    return false;
  }

  actor_t::actor_t(std::unique_ptr<role_t>&& role)
  : role(std::move(role)),
    id(-1),
    sick(false)
  {
    this->name = this->role->DefaultName();
  }

  actor_t::~actor_t()
  {
    assert(this->ID() == -1);
    assert(this->mailbox.Empty());
  }

} // namespace bb
