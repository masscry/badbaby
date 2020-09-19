/**
 * @file ecs.hpp
 * @author masscry
 * 
 * Simple ECS
 * 
 */

#pragma once
#ifndef STARRG_ECS_HEADER
#define STARRG_ECS_HEADER

#include <glm/vec2.hpp>
#include <cstdint>
#include <vector>
#include <list>
#include <deque>
#include <type_traits>
#include <algorithm>
#include <mutex>

namespace sr
{

  using entityID_t = int32_t;

  class basicComponentFactory_t
  {
    friend class entityFactory_t;
    virtual void OnEntityDelete(entityID_t id) = 0;
  public:
    virtual ~basicComponentFactory_t() = 0;
  };

  class entityFactory_t final
  {
    mutable std::mutex mutex;
    std::vector<bool> entity;
    std::list<entityID_t> released;
    std::list<basicComponentFactory_t*> factories;

    entityFactory_t();
    ~entityFactory_t();

    void ReleaseNoLock(entityID_t id);

  public:

    static entityFactory_t& Instance();

    void RegisterFactory(basicComponentFactory_t* factory);
    void RemoveFactory(basicComponentFactory_t* factory);

    bool Valid(entityID_t entityID) const;
    entityID_t GetNew();
    void Release(entityID_t id);

    void Clear();

    entityFactory_t(const entityFactory_t&) = delete;
    entityFactory_t& operator=(const entityFactory_t&) = delete;
    entityFactory_t(entityFactory_t&&) = delete;
    entityFactory_t& operator=(entityFactory_t&&) = delete;
  };

  template<class ForwardIt, class T, class Compare>
  ForwardIt BinarySearch(ForwardIt first, ForwardIt last, const T& value, Compare comp)
  {
    first = std::lower_bound(first, last, value, comp);
    if (!(first == last) && !(comp(value, *first)))
    {
      return first;
    }
    return last;
  }

  template<typename data_t>
  class component_t final
  {
    using self_t = component_t<data_t>;
    friend class std::deque<self_t>;

    static_assert(std::is_trivial<data_t>::value == true, "Data Must Be Trivial");

    entityID_t id;
    data_t data;

  public:

    bool operator < (const component_t<data_t>& rhs) const
    {
      return this->id < rhs.id;
    }

    class factory_t: public basicComponentFactory_t
    {
      using storage_t = std::deque<self_t>;

      struct compareSelfItems_t
      {
        bool operator()(entityID_t a, const self_t& b)
        {
          return a < b.ID();
        }
        bool operator()(const self_t& a, entityID_t b)
        {
          return a.ID() < b;
        }
        bool operator()(const self_t& a, const self_t& b)
        {
          return a.ID() < b.ID();
        }
      };

      mutable std::mutex mutex;
      storage_t storage;

      void OnEntityDelete(entityID_t id) override
      {
        std::unique_lock<std::mutex> lock(this->mutex);
        auto it = BinarySearch(this->storage.begin(), this->storage.end(), id, compareSelfItems_t());
        if (it != this->storage.end())
        {
          this->storage.erase(it);
        }
      }

      factory_t()
      {
        entityFactory_t::Instance().RegisterFactory(this);
      }

      ~factory_t() override
      {
        entityFactory_t::Instance().RemoveFactory(this);
      }

      self_t& ItemNoLock(entityID_t id)
      {
        auto it = BinarySearch(this->storage.begin(), this->storage.end(), id, compareSelfItems_t());
        if (it != this->storage.end())
        {
          return *it;
        }
        throw std::runtime_error("Item not found!");
      }

    public:

      template<typename func_t>
      void Each(func_t func)
      {
        for (auto it: storage)
        {
          func(it);
        }
      }

      static factory_t& Instance()
      {
        static self_t::factory_t factory;
        return factory;
      }

      self_t& NewComponent(entityID_t id, data_t data)
      {
        std::unique_lock<std::mutex> lock(this->mutex);
        auto it = BinarySearch(this->storage.begin(), this->storage.end(), id, compareSelfItems_t());
        if (it != this->storage.end())
        {
          return *it;
        }

        this->storage.emplace_back(
          id, data
        );
        std::sort(this->storage.begin(), this->storage.end(), compareSelfItems_t());

        return this->ItemNoLock(id);
      }

      self_t& NewComponent(entityID_t id)
      {
        return this->NewComponent(id, data_t());
      }

      const self_t& Item(entityID_t id) const
      {
        return const_cast<self_t::factory_t*>(this)->Item(id);
      }

      self_t& Item(entityID_t id)
      {
        std::unique_lock<std::mutex> lock(this->mutex);
        return this->ItemNoLock(id);
      }

      const self_t* OptionalItem(entityID_t id) const
      {
        return const_cast<self_t::factory_t*>(this)->OptionalItem(id);
      }

      self_t* OptionalItem(entityID_t id)
      {
        std::unique_lock<std::mutex> lock(this->mutex);
        auto it = BinarySearch(this->storage.begin(), this->storage.end(), id, compareSelfItems_t());
        if (it != this->storage.end())
        {
          return &(*it);
        }
        return nullptr;
      }

    };

    data_t* operator->()
    {
      return &this->data;
    }

    data_t& operator*()
    {
      return this->data;
    }

    const data_t* operator->() const
    {
      return &this->data;
    }

    const data_t& operator*() const
    {
      return this->data;
    }

    entityID_t ID() const
    {
      return this->id;
    }

    data_t& Data() 
    {
      return this->data;
    }

    const data_t& Data() const
    {
      return this->data;
    }

    component_t(entityID_t id, data_t data)
    : id(id),
      data(data)
    {
      ;
    }

    component_t(const component_t& comp)
    : id(comp.id),
      data(comp.data)
    {
      ;
    }

    component_t(component_t&& comp)
    : id(comp.id),
      data(std::move(comp.data))
    {
      ;
    }

    ~component_t()
    {
      ;
    }

    component_t& operator=(const component_t& comp)
    {
      if (this != &comp)
      {
        this->data = comp.data;
      }
      return *this;
    }

    component_t& operator=(component_t&& comp)
    {
      if (this != &comp)
      {
        this->data = std::move(comp.data);
      }
      return *this;
    }

  };

} // namespace sr

#endif /* STARRG_ECS_HEADER */