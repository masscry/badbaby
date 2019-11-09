#include <bson.hpp>
#include <common.hpp>

#include <cstdio>
#include <cstring>

#include <stdexcept>

namespace bb
{
  namespace bson
  {

    document_t::document_t()
    :self(std::make_shared<dataArray_t>())
    {
      this->self->push_back('\x00');
    }

    document_t::document_t(const document_t& cp)
    :self(std::make_shared<dataArray_t>(*cp.self))
    {
      ;
    }

    document_t::document_t(document_t&& mv)
    :self(std::move(mv.self))
    {
      ;
    }

    document_t& document_t::operator=(const document_t& cp)
    {
      if (this != &cp)
      {
        this->self = std::make_shared<dataArray_t>(*cp.self);
      }
      return *this;
    }

    document_t& document_t::operator=(document_t&& mv)
    {
      if (this != &mv)
      {
        this->self = std::move(mv.self);
      }
      return *this;
    }

    void document_t::Load(const std::string& filename)
    {
      FILE* input = fopen(filename.c_str(), "rb");
      if (input == nullptr)
      {
        throw std::runtime_error(std::string("Failed to open \"") + filename + "\" to read");
      }
      BB_DEFER(fclose(input));

      int32_t totalDocSize = 0;
      if (fread(&totalDocSize, sizeof(int32_t), 1, input) != 1)
      {
        throw std::runtime_error("Failed to read BSON document size");
      }

      dataArray_t newSelfData;
      newSelfData.resize(totalDocSize-4);

      auto dataIt = newSelfData.begin();
      totalDocSize -= sizeof(int32_t); // ignore size of totalDocSize
      while (totalDocSize-->0)
      {
        int tmp = fgetc(input);
        if (tmp == EOF)
        {
          throw std::runtime_error("Failed to read BSON data");
        }
        *dataIt++ = static_cast<uint8_t>(tmp&0xFF);
      }

      if (newSelfData[totalDocSize-1] != '\x00')
      {
        throw std::runtime_error("Invalid BSON document format");
      }
      this->self = std::make_shared<dataArray_t>(std::move(newSelfData));
    }

    void document_t::Store(const std::string& filename) const
    {
      FILE* output = fopen(filename.c_str(), "wb");
      if (output == nullptr)
      {
        throw std::runtime_error(std::string("Failed to open \"") + filename + "\" for writing");
      }
      BB_DEFER(fclose(output));

      int32_t totalDocSize = this->self->size() + sizeof(int32_t);
      if (fwrite(&totalDocSize, sizeof(int32_t), 1, output) != 1)
      {
        throw std::runtime_error("Failed to write BSON document size");
      }

      for (auto dataIt = this->self->begin(), dataEnd = this->self->end(); dataIt != dataEnd; ++dataIt)
      {
        if (fputc(*dataIt, output) == EOF)
        {
          throw std::runtime_error("Failed to write BSON document data");
        }
      }
    }




    
  } // namespace bson

} // namespace bb
