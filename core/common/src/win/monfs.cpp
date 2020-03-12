#include <monfs.hpp>
#include <common.hpp>

#include <cassert>
#include <cstdio>

namespace bb
{

  namespace fs
  {

    processor_t::~processor_t()
    {
      ;
    }
    
    bool monitor_t::IsGood() const
    {
      return static_cast<bool>(this->processor);
    }

    int monitor_t::Check()
    {
      if (!this->IsGood())
      { // Programmer's error!
        assert(0);
        return -1;
      }

	  return 0;
    }

    int monitor_t::Watch(const char* filename)
    {
      if (!this->IsGood())
      { // Programmer's error!
        assert(0);
        return -1;
      }
      return 0;
    }

    int monitor_t::RemoveWatch(int wd)
    {
      if (!this->IsGood())
      { // Programmer's error!
        assert(0);
        return -1;
      }
      return 0;
    }

    monitor_t::monitor_t()
    : self(-1)
    {

    }

    monitor_t::monitor_t(int self, std::unique_ptr<processor_t>&& processor)
    : self(self),
      processor(std::move(processor))
    {
      ;
    }

    monitor_t::monitor_t(monitor_t&& monitor)
    : self(monitor.self),
      processor(std::move(monitor.processor))
    {
      monitor.self = -1;
    }
    
    monitor_t& monitor_t::operator=(monitor_t&& monitor)
    {
      if (this != &monitor)
      {
        if (this->self != -1)
        {
			// Programmer's mistake!
			assert(0);
        }

        this->self = monitor.self;
        this->processor = std::move(monitor.processor);

        monitor.self = -1;
      }
      return *this;
    }

    monitor_t::~monitor_t()
    {
      if (this->self != -1)
      {
		  // Programmer's mistake!
		  assert(0);
	  }
    }

    monitor_t monitor_t::Create(
      std::unique_ptr<processor_t>&& processor
    )
    {
      if (!processor)
      {
        // Programmer's error!
        // No need to create monitor without processor
        assert(0);
        return monitor_t();
      }
      return monitor_t(-1, std::move(processor));
    }

  } // namespace fs

} // namespace bb
