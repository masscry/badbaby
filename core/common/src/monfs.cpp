#include <monfs.hpp>
#include <common.hpp>

#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cassert>
#include <cstring>
#include <climits>

#include <unistd.h>
#include <sys/types.h>
#include <sys/inotify.h>

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
      return (this->self != -1) && (this->processor);
    }

    int monitor_t::Check()
    {
      if (!this->IsGood())
      { // Programmer's error!
        assert(0);
        return -1;
      }

      fd_set desc;

      FD_ZERO(&desc);
      FD_SET(this->self, &desc);

      struct timeval justPoll;

      justPoll.tv_sec = 0;
      justPoll.tv_usec = 0;

      auto dataIsReady = select(this->self + 1, &desc, nullptr, nullptr, &justPoll);

      if (dataIsReady == 0)
      { // Polling says - no change happened
        return 0;
      }

      if (dataIsReady < 0)
      { // Error happened!
        char buffer[1024];
        char* text = strerror_r(errno, buffer, sizeof(buffer));
        bb::Error("Error: inotify select failed \"%s\" (%d)", text, errno);
        return -1;
      }

      // here have data to read from inotify
      char notifyBuffer[1024];
      static_assert(
        sizeof(notifyBuffer) >= (sizeof(inotify_event) + NAME_MAX + 1),
        "Documentation says - notify events buffer must be at least this size"
      );

      auto notifyLen = read(this->self, notifyBuffer, sizeof(notifyBuffer));
      if (notifyLen == -1)
      {
        char buffer[1024];
        char* text = strerror_r(errno, buffer, sizeof(buffer));
        bb::Error("Error: inotify read failed \"%s\" (%d)", text, errno);
        return -1;
      }

      char* notifyCursor = notifyBuffer;
      int counter = 0;
      while (notifyLen > 0)
      {
        auto event = reinterpret_cast<inotify_event*>(notifyCursor);
        if ((event->mask & IN_MODIFY) != 0)
        {
          if (this->processor->OnChange(event->name, event_t::modified) != 0)
          {
            return -1;
          }
          ++counter;
        }
        else
        {
          bb::Debug("Got strange event: %u at %s", event->mask, event->name);
        }
        notifyLen -= (sizeof(inotify_event) + event->len);
        notifyCursor += (sizeof(inotify_event) + event->len);
      }
      return counter;
    }

    int monitor_t::Watch(const char* filename)
    {
      if (!this->IsGood())
      { // Programmer's error!
        assert(0);
        return -1;
      }

      int wd = inotify_add_watch(this->self, filename, IN_MODIFY);
      if (wd == -1)
      {
        char buffer[1024];
        char* text = strerror_r(errno, buffer, sizeof(buffer));
        bb::Error("inotify_add_watch failed \"%s\" (%d)", text, errno);
      }
      return wd;
    }

    int monitor_t::RemoveWatch(int wd)
    {
      if (!this->IsGood())
      { // Programmer's error!
        assert(0);
        return -1;
      }

      int result = inotify_rm_watch(this->self, wd);
      if (result == -1)
      {
        char buffer[1024];
        char* text = strerror_r(errno, buffer, sizeof(buffer));
        bb::Error("inotify_rm_watch failed \"%s\" (%d)", text, errno);
      }
      return result;
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
          close(this->self);
          this->self = -1;
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
        close(this->self);
        this->self = -1;
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

      int notifyHandle = inotify_init1(IN_NONBLOCK);
      if (notifyHandle == -1)
      {
        char buffer[1024];
        char* text = strerror_r(errno, buffer, sizeof(buffer));
        bb::Error("Error: inotify_init1 failed \"%s\" (%d)", text, errno);
        return monitor_t();
      }

      return monitor_t(notifyHandle, std::move(processor));
    }

  } // namespace fs

} // namespace bb
