/**
 * @file common.hpp
 *
 * Common Engine Routines
 */

#pragma once
#ifndef __BB_COMMON_HEADER__
#define __BB_COMMON_HEADER__

#include <cstdio>

#include <type_traits>
#include <limits>
#include <functional>
#include <memory>
#include <mutex>
#include <condition_variable>

//#define BB_DOUBLE_LOCK_ASSERT

#if defined(__clang__) || defined(__GNUC__)
#define BB_FORMAT_LIKE_PRINTF(N, M) __attribute__ ((format (printf, N, M)))
#else
#define BB_FORMAT_LIKE_PRINTF(N, M)
#endif

#ifndef _GNU_SOURCE
extern "C" int vasprintf(char** strp, const char* fmt, va_list ap);
#endif /* _GNU_SOURCE */

namespace bb
{

  template <typename T, std::size_t N>
  constexpr std::size_t countof(T const (&)[N]) noexcept
  {
    return N;
  }

  template<typename T>
  constexpr T breakingIndex()
  {
    return std::numeric_limits<T>::max();
  }

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

  void Debug(const char* format, ...) BB_FORMAT_LIKE_PRINTF(1, 2);
  void Info(const char* format, ...) BB_FORMAT_LIKE_PRINTF(1, 2);
  void Warning(const char* format, ...) BB_FORMAT_LIKE_PRINTF(1, 2);
  void Error(const char* format, ...) BB_FORMAT_LIKE_PRINTF(1, 2);

  template<typename data_t>
  data_t CheckValueBounds(data_t value, data_t lowerBound, data_t upperBound)
  {
    return (value < lowerBound) ? (lowerBound) :
      ((value > upperBound) ? (upperBound) : value);
  }

  const std::string& GetThisThreadName();

  void SetThisThreadName(const std::string& name);

  std::string GenerateUniqueName();

  std::string CurrentTime();

  template<typename type_t>
  inline constexpr type_t signum(type_t value, std::false_type)
  {
    return type_t(0) < value;
  }

  template<typename type_t>
  inline constexpr type_t signum(type_t value, std::true_type)
  {
    return (type_t(0) < value) - (value < type_t(0));
  }

  template<typename type_t>
  inline constexpr type_t signum(type_t value)
  {
    return bb::signum(value, std::is_signed<type_t>());
  }

  #ifdef BB_DOUBLE_LOCK_ASSERT
    #define READ_MODE ("R")
    #define WRITE_MODE ("W")

    void AddLock(const std::string& thread, const std::string& mutex, const char* mode);
    void RemoveLock(const std::string& thread, const std::string& mutex, const char* mode);
  #else
    #define AddLock(THREAD, MUTEX, MODE)
    #define RemoveLock(THREAD, MUTEX, MODE)
  #endif /* BB_DOUBLE_LOCK_ASSERT */

  class rwMutex_t final
  {
    std::mutex              mutex;
    std::condition_variable gate1;
    std::condition_variable gate2;

    bool                    hasWriter;
    unsigned int            totalReaders;

    #ifdef BB_DOUBLE_LOCK_ASSERT
    std::string uid;
    #endif /* BB_DOUBLE_LOCK_ASSERT */

    rwMutex_t(const rwMutex_t&) = delete;
    rwMutex_t(rwMutex_t&&) = delete;

    rwMutex_t& operator=(const rwMutex_t&) = delete;
    rwMutex_t& operator=(rwMutex_t&&) = delete;

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

    void LockWrite();
    void UnlockWrite();

    void LockRead();

    void UnlockRead();

    rwMutex_t()
    :hasWriter(false), totalReaders(0)
    #ifdef BB_DOUBLE_LOCK_ASSERT
    ,uid(GenerateUniqueName())
    #endif /* BB_DOUBLE_LOCK_ASSERT */
    {
      ;
    }

    ~rwMutex_t() = default;

    using readLock_t = std::unique_ptr<rwMutex_t, releaseReadLock_t>;
    using writeLock_t = std::unique_ptr<rwMutex_t, releaseWriteLock_t>;

    readLock_t GetReadLock()
    {
      this->LockRead();
      return readLock_t(this);
    }

    writeLock_t GetWriteLock()
    {
      this->LockWrite();
      return writeLock_t(this);
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

    callOnScopeExit(callOnScopeExit&& other) noexcept
    :func(std::move(other.func))
    {
      other.func = nullptr;
    }

    callOnScopeExit& operator = (callOnScopeExit&& other) noexcept
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

  inline void rwMutex_t::LockWrite()
  {
    std::unique_lock<std::mutex> lock(this->mutex);

    AddLock(GetThisThreadName(), this->uid, WRITE_MODE);

    this->gate1.wait(lock, [this](){ return !this->hasWriter; });
    this->hasWriter = true;
    this->gate2.wait(lock, [this](){ return this->totalReaders == 0; });
  }

  inline void rwMutex_t::UnlockWrite()
  {
    {
      std::lock_guard<std::mutex> lock(this->mutex);
      this->hasWriter = false;
      this->totalReaders = 0;
      RemoveLock(GetThisThreadName(), this->uid, WRITE_MODE);
    }
    this->gate1.notify_all();
  }

  inline void rwMutex_t::LockRead()
  {
    std::unique_lock<std::mutex> lock(this->mutex);
    AddLock(GetThisThreadName(), this->uid, READ_MODE);
    this->gate1.wait(lock,
      [this]()
      { // when already writer in queue, or too many readers
        return (!this->hasWriter) && (this->totalReaders != std::numeric_limits<decltype(this->totalReaders)>::max()); 
      }
    );
    ++this->totalReaders;
  }

  inline void rwMutex_t::UnlockRead()
  {
    std::lock_guard<std::mutex> lock(this->mutex);
    --this->totalReaders;
    RemoveLock(GetThisThreadName(), this->uid, READ_MODE);
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


}

#define BB_CALL_SCOPE_NAME_1(PREFIX, INDEX) PREFIX ## INDEX
#define BB_CALL_SCOPE_NAME_2(PREFIX, INDEX) BB_CALL_SCOPE_NAME_1(PREFIX, INDEX)
#define BB_CALL_SCOPE_NAME_3(PREFIX) BB_CALL_SCOPE_NAME_2(PREFIX, __COUNTER__)
#define BB_DEFER(CODE) auto BB_CALL_SCOPE_NAME_3(_bb_defer_) = bb::callOnScopeExit([&](){ CODE; })

#if defined(__LP64__) && defined(__linux__)
#define BBsize_t "%zu"
#define BBssize_t "%zd"
#endif

#if _MSC_VER >= 1800
#define BBsize_t "%zu"
#define BBssize_t "%zd"
#endif

#if defined(__APPLE__)
#define BBsize_t "%zu"
#define BBssize_t "%zd"
#endif

#ifndef BBsize_t
#error "Unsupported platform! Must defined BBsize_t for size_t types"
#endif /* BBsize_t */

#endif /* __BB_COMMON_HEADER__ */
