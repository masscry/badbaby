#pragma once
#ifndef __BB_EXT_BINSTORE_HEADER__
#define __BB_EXT_BINSTORE_HEADER__

#include <cstdio>

#include <type_traits>
#include <string>

namespace bb
{
  namespace ext
  {

    class binstore_t
    {
    public:

      enum class openMode_t {
        undef = -1,
        create = 0,
        read = 1
      };

    private:

      openMode_t om;
      FILE* stream;
      fpos_t headPos;
      uint32_t tag;
      bool dirty;
      size_t dataSize;

      binstore_t(FILE* stream, openMode_t om);

      int PutHeader();
      int GetHeader();

      int Setup();

      int Flush();

      int Write(const void* buffer, size_t bufferSize);

      int Read(void* buffer, size_t bufferSize);

    public:

      operator bool() const
      {
        return this->IsGood();
      }

      uint32_t Tag() const;

      int SetTag(uint32_t tag);

      int Write(const char* str);
      int Write(const std::string& str);
      int Read(std::string& str);

      template<typename data_t>
      int Write(data_t value)
      {
        static_assert(!std::is_pointer<data_t>::value, "Must be not a pointer");
        static_assert(std::is_trivial<data_t>::value, "Must be trivial");
        static_assert(std::is_standard_layout<data_t>::value, "Must has standard layout");
        return this->Write(
          &value, sizeof(data_t)
        );
      }

      template<typename data_t>
      int Read(data_t& value)
      {
        static_assert(!std::is_pointer<data_t>::value, "Must be not a pointer");
        static_assert(std::is_trivial<data_t>::value, "Must be trivial");
        static_assert(std::is_standard_layout<data_t>::value, "Must has standard layout");

        return this->Read(
          &value, sizeof(data_t) 
        );
      }

      bool Reset();

      bool IsGood() const;

      openMode_t OpenMode() const;

      binstore_t();
      ~binstore_t();

      static binstore_t Read(const char* filename);
      static binstore_t Create(const char* filename);

      binstore_t(binstore_t &&) noexcept;
      binstore_t &operator=(binstore_t &&) noexcept;

      binstore_t(const binstore_t&) = delete;
      binstore_t& operator=(const binstore_t&) = delete;

    };

  } // namespace ext
} // namespace bb






#endif /* __BB_EXT_BINSTORE_HEADER__ */