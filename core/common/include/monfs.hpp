/**
 * @file monfs.hpp
 *
 * Monitor Filesystem
 *
 */

#pragma once
#ifndef __BB_CORE_COMMON_MONITOR_FS_HEADER__
#define __BB_CORE_COMMON_MONITOR_FS_HEADER__

#include <memory>

namespace bb
{

  namespace fs
  {

    enum class event_t
    {
      modified
    };

    class processor_t
    {
    public:
      virtual int OnChange(const char* filename, event_t event) = 0;
    };

    class monitor_t final
    {
      int self;
      std::unique_ptr<processor_t> processor;

      monitor_t(const monitor_t&) = delete;
      monitor_t& operator=(const monitor_t&) = delete;

      monitor_t(int self, std::unique_ptr<processor_t>&& processor);

    public:

      bool IsGood() const
      {
        return (this->self != -1) && (this->processor);
      }

      int Check();

      int Watch(const char* filename);

      int RemoveWatch(int wd);

      monitor_t();
      monitor_t(monitor_t&&);
      monitor_t& operator=(monitor_t&&);
      ~monitor_t();

      static monitor_t Create(
        std::unique_ptr<processor_t>&& processor
      );

    };



  } // namespace fs

} // namespace bb

#endif /* __BB_CORE_COMMON_MONITOR_FS_HEADER__ */

