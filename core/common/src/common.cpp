#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>

#include <thread>
#include <string>

#include <unistd.h>

#include <common.hpp>

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

  const std::string& GetThisThreadName()
  {
    static thread_local std::string cachedResult;
    if (cachedResult.empty())
    {
      // @see https://linux.die.net/man/3/pthread_getname_np (length is restricted to 16 characters, including the terminating null byte)
      const size_t maxResultSize = 16; 
      char result[maxResultSize];
      if (pthread_getname_np(pthread_self(), result, maxResultSize) == 0)
      {
        cachedResult = result;
      }
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
    ~logger_t() = default;

    void Log(logLevel_t level, const char* format, va_list vl);

  public:
    static logger_t& Instance();
  };

  logger_t::logger_t()
  {
    file_t output { fopen((GetThisThreadName() + ".log").c_str(), "wt") };
    if (output)
    {
      fprintf(output.get(), "%s", "Log Started\n");
    }
  }

  logger_t& logger_t::Instance()
  {
    static thread_local logger_t self;
    return self;
  }

  void logger_t::Log(logLevel_t level, const char* format, va_list vl)
  {
    file_t output { fopen((GetThisThreadName() + ".log").c_str(), "at") };
    fprintf(output.get(), "%s\t", logText[level]);
    vfprintf(output.get(), format, vl);
    fputc('\n', output.get());
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