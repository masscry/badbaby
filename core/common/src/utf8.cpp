#include <cassert>
#include <stdexcept>

#include <utf8.hpp>

namespace
{

  const uint32_t utf8Mask[bb::UTF8_TOTAL] = 
  {
    0x3F, // tail has 6 bits to use   10XXXXXX
    0x7F, // 1-byte has 7 bits to use 0XXXXXXX
    0x1F, // 2-byte has 5 bits to use 110XXXXX
    0x0F, // 3-byte has 4 bits to use 1110XXXX
    0x07  // 4-byte has 3 bits to use 11110XXX
  };

  uint32_t utf8Octet(const char* str, int octets)
  {
    uint32_t result = 0;

    // read first octet
    int cursor = *str++;
    result |= cursor & utf8Mask[octets];

    // read left octets
    --octets;
    while(octets-->0) 
    {
        cursor = *str++;
        if (bb::utf8SymbolLength(cursor) != bb::UTF8_TAIL)
        { // Bad UTF8 symbol
          assert(0);
          return 0xFFFFFFFF;
        }
        result <<= 6;
        result |= cursor & utf8Mask[0];
    }
    return result;
  }


} // namespace


namespace bb
{

  utf8Length_t utf8SymbolLength(char smb)
  {
    if ((smb & 0x80) == 0) 
    {
        // 0XXXXXXX
        return UTF8_ONE;
    }
    if ((smb & 0xC0) == 0x80) 
    {
        // tail detected
        return UTF8_TAIL;
    }
    if((smb & 0xE0) == 0xC0) 
    {
        // 110XXXXX 10XXXXXX 10XXXXXX 10XXXXXX
        return UTF8_TWO;
    }
    if((smb & 0xF0) == 0xE0) 
    {
        // 1110XXXX 10XXXXXX 10XXXXXX 10XXXXXX
        return UTF8_THREE;
    }
    if((smb & 0xF8) == 0xF0) 
    {
        // 11110XXX 10XXXXXX 10XXXXXX 10XXXXXX
        return UTF8_FOUR;
    }
    // invalid character
    assert(0);
    return UTF8_ERROR;
  }

  uint32_t utf8GetOctet(const char* str)
  {
    return utf8Octet(str, utf8SymbolLength(str[0]));
  }

  size_t utf8len(const char* str)
  {
    const char* cursor = str;
    size_t result = 0;
    while(*cursor != 0)
    {
      int len = utf8SymbolLength(*cursor);
      if (len <= 0)
      { // hit non utf8 character
        assert(0);
        return result;
      }
      cursor += len;
    }
    return result;
  }

  utf8Symbols utf8extract(const char* str)
  {
    utf8Symbols result;
    const char* cursor = str;
    result.reserve(utf8len(str));

    while(*cursor != 0)
    {
      int octets = utf8SymbolLength(*cursor);
      if (octets <= 0)
      {
        throw std::runtime_error("Non-UTF8 string");
      }

      result.push_back(utf8Octet(cursor, octets));
      cursor += octets;
    }
    return result;
  }

} // namespace bb
