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

      /**
       * Callback executes when change on filesysem occures.
       * 
       * @return 0, if no errors, non-zero otherwise
       */
      virtual int OnChange(const char* filename, event_t event) = 0;
      virtual ~processor_t() = 0;
    };

    class monitor_t final
    {
      int self;
      std::unique_ptr<processor_t> processor;

      monitor_t(const monitor_t&) = delete;
      monitor_t& operator=(const monitor_t&) = delete;

      monitor_t(int self, std::unique_ptr<processor_t>&& processor);

    public:

      bool IsGood() const;

      /**
       * Check for occured events.
       * 
       * If any callback return non-zero function return -1 immediately
       */
      int Check();

      /**
       * Add file to watch list
       * 
       * @param filename file to add to watch list
       * 
       * @return file ID in watch list or -1 on errors
       */
      int Watch(const char* filename);

      /**
       * Remove given file from watch list
       * 
       * @param wd file ID in watch list
       */
      int RemoveWatch(int wd);

      monitor_t();
      monitor_t(monitor_t&&);
      monitor_t& operator=(monitor_t&&);
      ~monitor_t();

      static monitor_t Create(
        std::unique_ptr<processor_t>&& processor
      );

      template<typename procImpl_t, typename... args_t>
      static monitor_t Create(
        args_t&& ... args
      )
      {
        static_assert(std::is_base_of<processor_t, procImpl_t>::value,
          "Can be used only with processor_t subclasses"
        );
        return monitor_t::Create(
          std::unique_ptr<processor_t>(
            new procImpl_t(std::forward(args)...)
          )
        );
      }

    };



  } // namespace fs

} // namespace bb

#endif /* __BB_CORE_COMMON_MONITOR_FS_HEADER__ */

