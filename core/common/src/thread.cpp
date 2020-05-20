#include <common.hpp>

#include <string>

#ifdef _WIN32

#include <unordered_map>
#include <mutex>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

namespace bb
{

#if defined(__APPLE__) || defined(__linux__)

  const std::string& GetThisThreadName()
  {
    static thread_local std::string cachedResult;
    if (cachedResult.empty())
    {
      // @see https://linux.die.net/man/3/pthread_getname_np (length is restricted to 16 characters, including the terminating null byte)
      std::array<char, 16> result;
      if (pthread_getname_np(pthread_self(), result.data(), result.size()) == 0)
      {
        cachedResult = std::string(result.data());
      }
    }
    return cachedResult;
  }

#define HAS_GetThisThreadName

#endif /* __APPLE__ || __linux__ */

#ifdef __APPLE__

  void SetThisThreadName(const std::string& name)
  {
    // macOS threads can set names only to themselves
    pthread_setname_np(name.c_str());
  }
#define HAS_SetThisThreadName
#endif

#ifdef __linux__
  void SetThisThreadName(const std::string& name)
  {
    pthread_setname_np(pthread_self(), name.c_str());
  }
#define HAS_SetThisThreadName
#endif

#ifdef _WIN32
  /** @see https://docs.microsoft.com/ru-ru/visualstudio/debugger/how-to-set-a-thread-name-in-native-code?view=vs-2019 */
  /* Threre are two ways to implement this functionality:
   *  1) SetThreadDescription available from Windows10
   *  2) Throw special exception (available only in debugger)
   *
   * I copied (2), because it is still better, than have no thread name at all
   * and there is no way I can be sure, that engine won't be used in older Windows.
   *
   * Log names depends on thread names, but actually - it is all just for debugging
   *
   */

  namespace
  {

    static const DWORD MS_VC_EXCEPTION = 0x406D1388;
#pragma pack(push,8)
    typedef struct tagTHREADNAME_INFO
    {
      DWORD dwType; // Must be 0x1000.
      LPCSTR szName; // Pointer to name (in user addr space).
      DWORD dwThreadID; // Thread ID (-1=caller thread).
      DWORD dwFlags; // Reserved for future use, must be zero.
    } THREADNAME_INFO;
#pragma pack(pop)

    void SetThisThreadNameForDebugger(const std::string& name)
    {
      if (IsDebuggerPresent() != FALSE)
      {
        THREADNAME_INFO info;
        info.dwType = 0x1000;
        info.szName = name.c_str();
        info.dwThreadID = -1;
        info.dwFlags = 0;
#pragma warning(push)
#pragma warning(disable: 6320 6322)
        __try
        {
          RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
          ;
        }
#pragma warning(pop)
      }
    }

  }

  static std::mutex threadNamesMutex;
  static std::unordered_map<std::thread::id, std::string> threadNames;

  void SetThisThreadName(const std::string& name)
  {
    // this part for internal bb mechanism (logs, etc...)
    std::lock_guard<std::mutex> guard(threadNamesMutex);
    threadNames[std::this_thread::get_id()] = name;

    // this part for debugger names support
    SetThisThreadNameForDebugger(name);
  }

  const std::string& GetThisThreadName()
  {
    static thread_local std::string cachedResult;
    if (cachedResult.empty())
    {
      std::lock_guard<std::mutex> guard(threadNamesMutex);
      auto name = threadNames.find(std::this_thread::get_id());
      if (name != threadNames.end())
      {
        cachedResult = name->second;
      }
    }
    return cachedResult;
  }

#define HAS_SetThisThreadName
#define HAS_GetThisThreadName

#endif

#ifndef HAS_SetThisThreadName
#error "SetThisThreadName undefined for given platform!"
#endif /* HAS_SetThisThreadName */

#ifndef HAS_GetThisThreadName
#error "GetThisThreadName undefined for given platform!"
#endif /* HAS_GetThisThreadName */

}