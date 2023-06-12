#include<ecs.hpp>
#include<components.hpp>
#include<common.hpp>

namespace sr
{

  entityFactory_t::entityFactory_t()
  {
    ;
  }

  entityFactory_t::~entityFactory_t()
  {
    ;
  }

  entityFactory_t& entityFactory_t::Instance()
  {
    static entityFactory_t self;
    return self;
  }

  bool entityFactory_t::Valid(entityID_t entityID) const
  {
    std::unique_lock<std::mutex> lock(this->mutex);
    if (entityID < 0)
    {
      return false;
    }
    if (auto eid = static_cast<size_t>(entityID); eid < this->entity.size())
    {
      return this->entity[eid];
    }
    return false;
  }

  entityID_t entityFactory_t::GetNew()
  {
    std::unique_lock<std::mutex> lock(this->mutex);
    if (this->released.empty())
    {
      this->entity.emplace_back(true);
      return static_cast<entityID_t>(this->entity.size()-1);
    }

    auto resultID = static_cast<size_t>(this->released.front());
    this->released.pop_front();

    this->entity[resultID] = true;
    return static_cast<entityID_t>(resultID);
  }

  void entityFactory_t::Release(entityID_t id)
  {
    std::unique_lock<std::mutex> lock(this->mutex);
    this->ReleaseNoLock(id);
  }

  void entityFactory_t::ReleaseNoLock(entityID_t id)
  {
    if ((id < 0) || (this->entity.size() < static_cast<size_t>(id)))
    {
      BB_PANIC();
      throw std::runtime_error("Try to release invalid ID");
    }

    for (auto factory: this->factories)
    {
      factory->OnEntityDelete(id);
    }

    this->entity[static_cast<size_t>(id)] = false;
    this->released.push_back(id);
  }

  void entityFactory_t::Clear()
  {
    std::unique_lock<std::mutex> lock(this->mutex);
    entityID_t index = 0;
    for (auto it = this->entity.begin(), e = this->entity.end(); it != e; ++it, ++index)
    {
      if (*it)
      {
        this->ReleaseNoLock(index);
      }
    }
  }

  void entityFactory_t::RegisterFactory(basicComponentFactory_t* factory)
  {
    std::unique_lock<std::mutex> lock(this->mutex);
    this->factories.push_back(factory);
  }

  void entityFactory_t::RemoveFactory(basicComponentFactory_t* factory)
  {
    std::unique_lock<std::mutex> lock(this->mutex);
    for (auto it = this->factories.begin(), e = this->factories.end(); it != e; ++it)
    {
      if (factory == *it)
      {
        this->factories.erase(it);
        return;
      }
    }
  }

  basicComponentFactory_t::~basicComponentFactory_t()
  {
    ;
  }

} // namespace sr
