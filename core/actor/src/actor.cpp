#include <actor.hpp>
#include <worker.hpp>

#include <cassert>

namespace bb
{

  actor_t::actor_t()
  : name("actor_t"),
    id(-1),
    pool(nullptr)
  {
    ;
  }

  actor_t::actor_t(const actor_t& src)
  : name(src.name),
    id(-1), // copy of actor is not registered
    pool(nullptr)
  {
    ;
  }

  actor_t::actor_t(actor_t&& src)
  : name(std::move(src.name)),
    id(src.id),
    pool(src.pool)
  {
    src.id = -1;
    src.pool = nullptr;
  }

  void actor_t::Unregister()
  {
    if (this->pool != nullptr)
    {
      this->pool->Unregister(this->id);
      this->pool = nullptr;
      this->id = -1;
    }
    assert(this->id == -1);
  }

  actor_t& actor_t::operator= (const actor_t& src)
  {
    if (this != &src)
    {
      this->Unregister();
      this->name = src.name;
      this->id = -1;
    }
    return *this;
  }

  actor_t& actor_t::operator =(actor_t&& src)
  {
    if (this != &src)
    {
      this->Unregister();

      this->name = std::move(src.name);
      this->id = src.id;
      this->pool = src.pool;

      src.id = -1;
      src.pool = nullptr;
    }
    return *this;
  }

  actor_t::~actor_t()
  { 
    // Expect actor to be deregistered before destruction
    assert(this->id == -1);
    if (this->pool != nullptr)
    { // we can forgive such mistake in release, but better fix this!
      this->pool->Unregister(this->id);
    }
  }

  void actor_t::PostMessageAsMe(int actorID, msg_t msg)
  {
    if ((this->pool == nullptr) || (this->id == -1))
    {
      throw std::runtime_error("Actor is not registered!");
    }
    msg.src = this->id;
    this->pool->PostMessage(actorID, msg);
  }

  void actor_t::ProcessMessage(msg_t msg)
  {
    this->OnProcessMessage(msg);
  }

} // namespace bb
