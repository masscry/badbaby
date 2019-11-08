/**
 * @file common.hpp
 * 
 * Common Engine Routines
 */

#pragma once
#ifndef __BB_COMMON_HEADER__
#define __BB_COMMON_HEADER__

#include <cstdio>
#include <memory>

#include <functional>
#include <mutex>
#include <condition_variable>

namespace bb
{

  /**
   * @brief Process common startup arguments.
   * 
   * Function can exit process, when invalid argument or -h flag received
   * 
   * @param argc total argument count
   * @param argv arguments array
   * @return int non-zero on errors, zero otherwise
   */
  int ProcessStartupArguments(int argc, char* argv[]);

  void Debug(const char* format, ...) __attribute__ ((format (printf, 1, 2)));
  void Info(const char* format, ...) __attribute__ ((format (printf, 1, 2)));
  void Warning(const char* format, ...) __attribute__ ((format (printf, 1, 2)));
  void Error(const char* format, ...) __attribute__ ((format (printf, 1, 2)));

  const std::string& GetThisThreadName();

  std::string CurrentTime();

  class rwMutex_t final
  {
    std::mutex              mutex;
    std::condition_variable gate1;
    std::condition_variable gate2;

    bool                    hasWriter;
    unsigned int            totalReaders;

    rwMutex_t(const rwMutex_t&) = delete;
    rwMutex_t(rwMutex_t&&) = delete;

    rwMutex_t& operator=(const rwMutex_t&) = delete;
    rwMutex_t& operator=(rwMutex_t&&) = delete;

    void LockWrite()
    {
      std::unique_lock<std::mutex> lock(this->mutex);
      this->gate1.wait(lock, [this](){ return !this->hasWriter; });
      this->hasWriter = true;
      this->gate2.wait(lock, [this](){ return this->totalReaders == 0; });
    }

    void UnlockWrite()
    {
      {
        std::lock_guard<std::mutex> lock(this->mutex);
        this->hasWriter = false;
        this->totalReaders = 0;
      }
      this->gate1.notify_all();
    }

    void LockRead()
    {
      std::unique_lock<std::mutex> lock(this->mutex);
      this->gate1.wait(lock,
        [this]()
        { // when already writer in queue, or too many readers
          return (!this->hasWriter) && (this->totalReaders != std::numeric_limits<decltype(this->totalReaders)>::max()); 
        }
      );
      ++this->totalReaders;
    }

    void UnlockRead()
    {
      std::lock_guard<std::mutex> lock(this->mutex);
      --this->totalReaders;
      if (this->hasWriter)
      {
        if (this->totalReaders == 0)
        {
          this->gate2.notify_one();
        }
      }
      else
      {
        if (this->totalReaders == std::numeric_limits<decltype(this->totalReaders)>::max()-1)
        {
          this->gate1.notify_one();
        }
      }
    }

    struct releaseReadLock_t final {
      void operator()(rwMutex_t* mut){
        mut->UnlockRead();
      }
    };

    struct releaseWriteLock_t final {
      void operator()(rwMutex_t* mut){
        mut->UnlockWrite();
      }
    };

  public:

    rwMutex_t()
    :hasWriter(false), totalReaders(0)
    {
      ;
    }

    ~rwMutex_t() = default;

    using readLock_t = std::unique_ptr<rwMutex_t, releaseReadLock_t>; 
    using writeLock_t = std::unique_ptr<rwMutex_t, releaseWriteLock_t>; 

    readLock_t GetReadLock() 
    {
      this->LockRead();
      return std::move(readLock_t(this));
    }

    writeLock_t GetWriteLock()
    {
      this->LockWrite();
      return std::move(writeLock_t(this));
    }

  };

  class callOnScopeExit final
  {
    std::function<void()> func;

    callOnScopeExit(const callOnScopeExit&) = delete;
    callOnScopeExit& operator=(const callOnScopeExit&) = delete;

  public:

    template<typename FUNC_TYPE>
    callOnScopeExit(FUNC_TYPE&& func)
    :func(std::forward<FUNC_TYPE>(func))
    {
      ;
    }

    callOnScopeExit(callOnScopeExit&& other)
    :func(std::move(other.func))
    {
      other.func = nullptr;
    }

    callOnScopeExit& operator = (callOnScopeExit&& other)
    {
      if (this == &other)
      {
        return *this;
      }

      if (this->func)
      {
        this->func();
      }

      this->func = std::move(other.func);
      other.func = nullptr;
      return *this;
    }

    ~callOnScopeExit()
    {
      if (this->func)
      {
        this->func();
      }
    }

  };

}

#define BB_CALL_SCOPE_NAME_1(PREFIX, INDEX) PREFIX ## INDEX
#define BB_CALL_SCOPE_NAME_2(PREFIX, INDEX) BB_CALL_SCOPE_NAME_1(PREFIX, INDEX)
#define BB_CALL_SCOPE_NAME_3(PREFIX) BB_CALL_SCOPE_NAME_2(PREFIX, __COUNTER__)
#define BB_DEFER(CODE) auto BB_CALL_SCOPE_NAME_3(_bb_defer_) = bb::callOnScopeExit([&](){ CODE; })

#endif /* __BB_COMMON_HEADER__ */
