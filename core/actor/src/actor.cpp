#include <actor.hpp>
#include <worker.hpp>
#include <role.hpp>

#include <cassert>

namespace bb
{

  msg_t IssuePoison()
  {
    return MakeMsg(-1, msgID_t::POISON, 0);
  }

  msg_t IssueSetID(int id)
  {
    return MakeMsg(-1, msgID_t::SETID, id);
  }

  msg_t IssueSetName(const char* name)
  {
    size_t nameLen = strlen(name);
    assert(nameLen <= msgDataByteLength);

    msg_t result;

    result.src = -1;
    result.type = msgID_t::SETNAME;
    memset(result.data, ' ', msgDataByteLength);
    memcpy(result.data, name, nameLen);
    return result;
  }

  msgResult_t actor_t::ProcessMessages()
  {
    std::unique_lock<std::mutex> inProcessLock(this->inProcess, std::try_to_lock);
    if (!inProcessLock.owns_lock())
    {
      return msgResult_t::skipped;
    }

    if (this->sick)
    {
      return msgResult_t::skipped;
    }

    auto& curRole = *this->role;

    size_t msgAlreadyInQueue = this->mailbox.Has();
    if (msgAlreadyInQueue == 0)
    {
      return msgResult_t::skipped;
    }

    auto result = msgResult_t::complete;
    while(msgAlreadyInQueue-->0)
    {
      auto msg = this->mailbox.Wait();
      switch(msg.type)
      {
      case msgID_t::SETNAME:
        {
          char tmpBuf[msgDataByteLength+1];
          memcpy(tmpBuf, msg.data, msgDataByteLength);
          tmpBuf[msgDataByteLength] = 0;
          this->name = tmpBuf;
        }
        break;
      case msgID_t::SETID:
        this->id = GetMsgData<int>(msg);
        break;
      case msgID_t::POISON:
        {
          bb::Debug("Actor \"%s\" (%d) is poisoned", this->Name().c_str(), this->ID());
          this->sick = true;
          this->id = -1;
          return msgResult_t::poisoned;
        }
        break;
      default:
        {
          auto tmpResult = curRole.ProcessMessage(*this, msg);
          if (tmpResult == msgResult_t::poisoned)
          {
            bb::Debug("Actor \"%s\" (%d) poisoned himself", this->Name().c_str(), this->ID());
            this->sick = true;
            this->id = -1;
            return msgResult_t::poisoned;
          }
          if (tmpResult != msgResult_t::complete)
          {
            result = tmpResult;
          }
        }
      }
    }
    return result;
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
