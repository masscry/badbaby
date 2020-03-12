#include <common.hpp>

#include <cstdarg>
#include <cstring>
#include <cstdio>

extern "C" int vasprintf(char** strp, const char* fmt, va_list ap)
{
  va_list apCopy;
  va_copy(apCopy, ap);
  BB_DEFER(va_end(apCopy));
  
  auto resultSize = _vscprintf(fmt, apCopy);
  auto resultStr = (char*) malloc(resultSize + 1);
  if (resultStr == nullptr)
  {
    return -1;
  }

  auto result = vsprintf_s(resultStr, resultSize + 1, fmt, ap);
  if (result < 0)
  {
    free(resultStr);
  }
  return result;
}

extern "C" void sincosf(float x, float* sin, float* cos)
{
  *sin = sinf(x);
  *cos = cosf(x);
}
