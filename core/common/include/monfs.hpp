/**
 * @file monfs.hpp
 *
 * Monitor Filesystem
 *
 */

#pragma once
#ifndef __BB_CORE_COMMON_MONITOR_FS_HEADER__
#define __BB_CORE_COMMON_MONITOR_FS_HEADER__

namespace bb
{

  namespace fs
  {

    enum class event_t
    {
      modified
    }

    class monitor_t
    {
      int self;

      monitor_t(const monitor_t&) = delete;
      monitor_t& operator=(const monitor_t&) = delete;

      virtual int OnChange(const char* filename, event_t event) = 0;

      monitor_t(int self);

    public:

      int Check();

      int Watch(const char* filename);

      monitor_t();
      monitor_t(monitor_&&);
      monitor_t& operator=(monitor_&&);
      virtual ~monitor_t();

      static monitor_t CreateMonitor(const char* filename);

    };

  } // namespace fs

} // namespace bb

#endif /* __BB_CORE_COMMON_MONITOR_FS_HEADER__ */

