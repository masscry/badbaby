#include <monfs.hpp>

#include <cstdio>
#include <cstdlib>
#include <cerrno>

#include <sys/types.h>
#include <linux/inotify.h>

namespace bb
{

  namespace fs
  {

    int monitor_t::Check()
    {
      char buffer[1024];

      


    }

    int monitor_t::Watch(const char* filename)
    {
      int result = inotify_add_watch(this->self, filename, IN_MODIFY);
      if (result == -1)
      {
        bb::Error("Error: inotify_add_watch failed %s (%d)", strerror(errno), errno);
        return -1;
      }
      return result;
    }

    monitor_t::monitor_t()
    : self(-1)
    {

    }

    monitor_t::monitor_t(int self)
    : self(self)
    {
      ;
    }

    monitor_t::~monitor_t()
    {
      if (this->self != -1)
      {
        close(this->self);
        this->self = -1;
      }
    }

    monitor_t monitor_t::CreateMonitor(const char* filename)
    {
      int notifyHandle = inotify_init1(IN_NONBLOCK);
      if (notifyHandle < 0)
      {
        bb::Error("Error: inotify_init failed %s (%d)", strerror(errno), errno);
        return monitor_t();
      }

      monitor_t result(notifyHandle);

      if (result.Watch(filename) == -1)
      {
        return monitor_t();
      }
      return result;
    }

  } // namespace fs

} // namespace bb
