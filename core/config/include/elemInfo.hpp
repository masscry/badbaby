/**
 * @file elemInfo.hpp
 * 
 * Helper classes for BSON type checking
 * 
 */

#pragma once
#ifndef __BB_CORE_CONFIG_ELEMENT_INFO_HEADER__
#define __BB_CORE_CONFIG_ELEMENT_INFO_HEADER__

#include <string>

#include <cstring>

namespace bb
{

  namespace bson
  {

    enum class type_t
    {
      end = '\x00',
      number = '\x01',
      string = '\x02',
      document = '\x03'
    };

    template<typename elem_t>
    struct elemInfo_t;

    template<>
    struct elemInfo_t<double>
    {
      enum { typeID = static_cast<int>(type_t::number) };

      template<typename cursor_t, typename iterator_t>
      static double extract(cursor_t cursor, iterator_t /*it*/)
      {
        uint8_t data[sizeof(double)];
        for (unsigned int i = 0; i < sizeof(double); ++i)
        {
          data[i] = *cursor++;
        }

        double result;
        memcpy(&result, data, sizeof(double));
        return result;
      }

      template<typename iterator_t>
      static iterator_t next(iterator_t cursor)
      {
        while(*cursor++ != '\x00'); // skip key

        for (unsigned int i = 0; i < sizeof(double); ++i)
        {
          ++cursor;
        }
        return cursor;
      }

    };
    
    template<typename iterator_t>
    iterator_t extractSize(iterator_t cursor, int32_t* result)
    {
      uint8_t resultBytes[sizeof(int32_t)];
      for (unsigned int i = 0; i < sizeof(int32_t); ++i)
      {
        resultBytes[i] = *cursor++;
      }
      memcpy(result, resultBytes, sizeof(int32_t));
      return cursor;
    }

    template<>
    struct elemInfo_t<std::string>
    {
      enum { typeID = static_cast<int>(type_t::string) };

      template<typename cursor_t, typename iterator_t>
      static std::string extract(cursor_t cursor, iterator_t /*it*/)
      {
        std::string result;
        int32_t size;

        cursor = extractSize(cursor, &size);
        --size; // remove trailing zero
        result.reserve(size);

        while((*cursor != '\x00') && (size-->0))
        {
          result.push_back(*cursor++);
        }
        if (size != 0)
        {
          throw std::runtime_error("Invalid BSON String");
        }
        return std::string(result);
      }

      template<typename iterator_t>
      static iterator_t next(iterator_t cursor)
      {
        while(*cursor++ != '\x00'); // skip key

        int32_t size;
        cursor = extractSize(cursor, &size);
        return cursor + size;
      }

    };

  } // namespace bson

} // namespace bb






#endif /* __BB_CORE_CONFIG_ELEMENT_INFO_HEADER__ */