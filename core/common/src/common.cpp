#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>

#include <thread>
#include <atomic>
#include <string>
#include <array>

#include <common.hpp>

#include <unordered_set>

#ifndef  _WIN32
#include <unistd.h>

#else

// WIN32 compatible functions

struct tm* localtime_r(const time_t* time, struct tm* tm)
{
	if (localtime_s(tm, time) != 0)
	{
		return nullptr;
	}
	return tm;
}

extern "C"
{
  extern thread_local int optopt;
  int getopt(int nargc, char* const* nargv, const char* ostr);
}

#endif // ! _WIN32

namespace
{

  void PrintHelp(FILE* output, const char* programName)
  {
    fprintf(output, "Usage: %s [-h]\n", programName);
    fprintf(output, "BadBaby Powered Game.\n\n");
    fprintf(output, "Standard supported arguments:\n");
    fprintf(output, "  -h\tPrint this message and exit\n");
  }

} // namespace

namespace bb
{

#ifdef BB_DOUBLE_LOCK_ASSERT
  using lockMap_t = std::unordered_set<std::string>;

  std::mutex guardLockMap;
  lockMap_t lockMap;  

  void AddLock(const std::string& thread, const std::string& mutex, const char* mode)
  {
    std::string key = thread+":"+mutex;
    std::unique_lock<std::mutex> lock(guardLockMap);

    Debug("LOCK: %s %s", mode, key.c_str());
    for (const auto& item: lockMap)
    {
      Debug("\t%s", item.c_str());
    }

    if (lockMap.find(key) != lockMap.end())
    {
      assert(0);
    }
    lockMap.insert(key);
  }

  void RemoveLock(const std::string& thread, const std::string& mutex, const char* mode)
  {
    std::string key = thread+":"+mutex;
    std::unique_lock<std::mutex> lock(guardLockMap);

    auto item = lockMap.find(key);
    if (item == lockMap.end())
    {
      assert(0);
    }
    lockMap.erase(item);

    Debug("UNLK: %s %s", mode,key.c_str());
    for (const auto& item: lockMap)
    {
      Debug("\t%s", item.c_str());
    }
  }
#endif /* BB_DOUBLE_LOCK_ASSERT */

  std::string CurrentTime()
  {
    time_t now = time(nullptr);

    struct tm nowtm;
    localtime_r(&now, &nowtm);

    std::array<char, 64> buffer;
    strftime(buffer.data(), buffer.size(), "%c", &nowtm);

    return std::string(buffer.data());
  }

  std::string GenerateUniqueName()
  {
    static std::atomic_int uidCounter(0);
    return std::string("uid") + std::to_string(uidCounter++);
  }

  const std::string& GetThisThreadName()
  {
    static thread_local std::string cachedResult;
    if (cachedResult.empty())
    {
#ifndef _WIN32
      // @see https://linux.die.net/man/3/pthread_getname_np (length is restricted to 16 characters, including the terminating null byte)
      std::array<char, 16> result;
      if (pthread_getname_np(pthread_self(), result.data(), result.size()) == 0)
      {
        cachedResult = std::string(result.data());
      }
#else
      std::hash<std::thread::id> hasher;
	  cachedResult = std::to_string(hasher(std::this_thread::get_id()));
#endif
    }
    return cachedResult;
  }

  int ProcessStartupArguments(int argc, char* argv[])
  {
    int option;

    while ((option = getopt(argc, argv, "h")) != -1)
    {
      switch (option)
      {
      case 'h':
        PrintHelp(stdout, argv[0]);
        exit(EXIT_SUCCESS);
        // code won't go here
        return 0;
      case ':':
        fprintf(stderr, "Option -%c requires parameter\n", optopt);
        PrintHelp(stderr, argv[0]);
        exit(EXIT_FAILURE);
        // code won't go here
        return -1;
      case '?':
        fprintf(stderr, "Unrecognized option -%c\n", optopt);
        PrintHelp(stderr, argv[0]);
        exit(EXIT_FAILURE);
        // code won't go here
        return -1;
      default:
        assert(0);
      }
    }

    return 0;
  }

  enum logLevel_t {
    LL_FIRST = 0,
    LL_DEBUG = LL_FIRST,
    LL_INFO,
    LL_WARNING,
    LL_ERROR,
    LL_TOTAL
  };

  const char* logText[LL_TOTAL] = {
    "[DBG]",
    "[INF]",
    "[WRN]",
    "[ERR]"
  };

  class logger_t
  {

    friend void Debug(const char* format, ...);
    friend void Info(const char* format, ...);
    friend void Warning(const char* format, ...);
    friend void Error(const char* format, ...);

    logger_t();
    logger_t(const logger_t&) = delete;
    logger_t(logger_t&&) = delete;
    logger_t& operator=(const logger_t&) = delete;
    logger_t& operator=(logger_t&&) = delete;
    ~logger_t();

    void Log(logLevel_t level, const char* format, va_list vl);

  public:
    static logger_t& Instance();
  };

  logger_t::logger_t()
  {
    FILE* output = fopen((GetThisThreadName() + ".log").c_str(), "wt");
    if (output != nullptr)
    {
      BB_DEFER(fclose(output));
      fprintf(output, "%s", "Log Started\n");
    }
  }

  logger_t::~logger_t()
  {
    FILE* output = fopen((GetThisThreadName() + ".log").c_str(), "at");
    if (output != nullptr)
    {
      BB_DEFER(fclose(output));
      fprintf(output, "%s", "Log Ended\n");
    }
  }

  logger_t& logger_t::Instance()
  {
    static thread_local logger_t self;
    return self;
  }

  void logger_t::Log(logLevel_t level, const char* format, va_list vl)
  {
    FILE* output = fopen((GetThisThreadName() + ".log").c_str(), "at");
    if (output != nullptr)
    {
      BB_DEFER(fclose(output));

      fprintf(output, "%s %s\t", CurrentTime().c_str(), logText[level]);
      vfprintf(output, format, vl);
      fputc('\n', output);
    }
  }

  void Debug(const char* format, ...)
  {
    va_list vl;
    va_start(vl, format);
    logger_t::Instance().Log(LL_DEBUG, format, vl);
    va_end(vl);
  }

  void Info(const char* format, ...)
  {
    va_list vl;
    va_start(vl, format);
    logger_t::Instance().Log(LL_INFO, format, vl);
    va_end(vl);
  }

  void Warning(const char* format, ...)
  {
    va_list vl;
    va_start(vl, format);
    logger_t::Instance().Log(LL_WARNING, format, vl);
    va_end(vl);
  }

  void Error(const char* format, ...)
  {
    va_list vl;
    va_start(vl, format);
    logger_t::Instance().Log(LL_ERROR, format, vl);
    va_end(vl);
  }

} // namespace bb