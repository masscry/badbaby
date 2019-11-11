/**
 * @file bson.hpp
 * 
 * Partial BSON printer/scanner.
 */

#pragma once
#ifndef __BB_CORE_CONFIG_BSON_HEADER__
#define __BB_CORE_CONFIG_BSON_HEADER__

#include <cstdint>

#include <string>
#include <deque>
#include <memory>

#include <elemInfo.hpp>

namespace bb
{
  namespace bson
  {

    class document_t final
    {
      using dataArray_t = std::deque<uint8_t>;
      using storage_t = std::shared_ptr<dataArray_t>;
      using weakStorage_t = std::weak_ptr<dataArray_t>;

      storage_t self;

      dataArray_t::const_iterator offset;
      size_t size;

    public:

      class iterator_t
      {
        friend inline bool operator == (const document_t::iterator_t& lhs, const document_t::iterator_t& rhs);

        weakStorage_t weakArr;
        dataArray_t::const_iterator offset;

      public:

        storage_t Document() const
        {
          return this->weakArr.lock();
        }

        iterator_t(weakStorage_t arr, dataArray_t::const_iterator offset);
        iterator_t(const iterator_t& cp);
        iterator_t& operator=(const iterator_t& cp);
        ~iterator_t() = default;

        type_t Type() const
        {
          auto arr = weakArr.lock();
          dataArray_t::const_iterator cursor = offset;
          switch(*cursor)
          {
            case '\x00':
            case '\x01':
            case '\x02':
            case '\x03':
              return static_cast<type_t>(*cursor);
            default:
              throw std::runtime_error(std::string("Unknown Data Type ") + std::to_string(*cursor));
          }
        }

        std::string Key() const;

        template<typename elem_t>
        elem_t Value() const
        {
          auto arr = weakArr.lock();
          dataArray_t::const_iterator cursor = this->offset;
          if (*cursor == '\x00')
          {
            throw std::runtime_error("Can't dereference iterator");
          }

          if (elemInfo_t<elem_t>::typeID != *cursor)
          {
            throw std::runtime_error("Invalid object type requested");
          }

          // skip key
          while(*cursor++ != '\x00');
          return elemInfo_t<elem_t>::extract(cursor, *this);
        }

        iterator_t& operator++();

        iterator_t operator++(int)
        {
          iterator_t tmp(*this);
          this->operator++();
          return tmp;
        }

      };

      iterator_t Begin() const;

      iterator_t End() const;

      void Load(const std::string& filename);
      void Store(const std::string& filename) const;

      document_t(const document_t& cp);
      document_t(document_t&& mv);

      document_t& operator=(const document_t& cp);
      document_t& operator=(document_t&& mv);

      document_t(storage_t doc, dataArray_t::const_iterator offset, size_t size);
      document_t();
      ~document_t() = default;
    };

    inline bool operator == (const document_t::iterator_t& lhs, const document_t::iterator_t& rhs)
    {
      return (lhs.offset == rhs.offset);
    }

    inline bool operator != (const document_t::iterator_t& lhs, const document_t::iterator_t& rhs)
    {
      return !(lhs == rhs);
    }

    //
    // document_t is not defined when elemInfo.hpp includes
    //

    template<>
    struct elemInfo_t<document_t>
    {
      enum { typeID = static_cast<int>(type_t::document) };

      template<typename cursor_t, typename iterator_t>
      static document_t extract(cursor_t cursor, iterator_t it)
      {
        int32_t size; // document size + size field
        cursor = extractSize(cursor, &size);
        size -= sizeof(int32_t);

        return document_t(it.Document(), cursor, size);
      }

      template<typename iterator_t>
      static iterator_t next(iterator_t cursor)
      {
        while(*cursor++ != '\x00'); // skip key

        int32_t size; // document size + size field
        cursor = extractSize(cursor, &size);
        return cursor + size - sizeof(int32_t);
      }

    };

  } // namespace bson
} // namespace bb

#endif /* __BB_CORE_CONFIG_BSON_HEADER__ */