#include <common.hpp>

#include <cstdarg>
#include <cstring>
#include <cstdio>
#include <cassert>

extern "C" int vasprintf(char** strp, const char* fmt, va_list ap)
{
  if ((strp == nullptr) || (fmt == nullptr))
  { // programmer's mistake
    assert(0);
    return -1;
  }

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

  *strp = resultStr;
  return result;
}
