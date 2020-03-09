/**
 * @file utf8.hpp
 * 
 * Common UTF-8 processing routines.
 * 
 */

#pragma once
#ifndef __BB_CORE_COMMON_UTF8_HEADER__
#define __BB_CORE_COMMON_UTF8_HEADER__

#include <cstddef>
#include <cstdint>

#include <vector>
#include <string>

namespace bb
{

  enum utf8Length_t
  {
    UTF8_ERROR = -1,
    UTF8_TAIL  =  0,
    UTF8_ONE,
    UTF8_TWO,
    UTF8_THREE,
    UTF8_FOUR,
    UTF8_TOTAL
  };

  using utf8Symbols = std::u32string;

  utf8Symbols utf8extract(const char* str); 

  utf8Length_t utf8SymbolLength(int smb);

  uint32_t utf8GetOctet(const char* str);

  size_t utf8len(const char* str);

} // namespace bb

#endif /* __BB_CORE_COMMON_UTF8_HEADER__ */