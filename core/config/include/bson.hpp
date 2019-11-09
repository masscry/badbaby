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

namespace bb
{
  namespace bson
  {

    class document_t final
    {
      using dataArray_t = std::deque<uint8_t>;
      using storage_t = std::shared_ptr<dataArray_t>;

      storage_t self;

    public:

      void Load(const std::string& filename);
      void Store(const std::string& filename) const;

      document_t(const document_t& cp);
      document_t(document_t&& mv);

      document_t& operator=(const document_t& cp);
      document_t& operator=(document_t&& mv);

      document_t();
      ~document_t() = default;
    };

  } // namespace bson
} // namespace bb

#endif /* __BB_CORE_CONFIG_BSON_HEADER__ */