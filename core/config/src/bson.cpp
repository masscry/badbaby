#include <bson.hpp>
#include <common.hpp>

#include <cstdio>
#include <cstring>

#include <stdexcept>

namespace bb
{
  namespace bson
  {

    document_t::iterator_t::iterator_t(weakStorage_t arr, dataArray_t::const_iterator offset)
    :weakArr(arr), offset(offset)
    {
      ;
    }

    document_t::iterator_t::iterator_t(const iterator_t& cp)
    :weakArr(cp.weakArr), offset(cp.offset)
    {

    }

    document_t::iterator_t& document_t::iterator_t::operator=(const iterator_t& cp)
    {
      if (this != &cp)
      {
        this->weakArr = cp.weakArr;
        this->offset = cp.offset;
      }
      return *this;
    }

    document_t::iterator_t& document_t::iterator_t::operator++()
    {
      auto arr = this->weakArr.lock();
      dataArray_t::const_iterator cursor = offset;
      switch (*cursor)
      {
        case '\x00':
          this->offset = arr->end();
          break;
        case elemInfo_t<double>::typeID:
          this->offset = elemInfo_t<double>::next(cursor);
          break;
        case elemInfo_t<std::string>::typeID:
          this->offset = elemInfo_t<std::string>::next(cursor);
          break;
        case elemInfo_t<document_t>::typeID:
          this->offset = elemInfo_t<document_t>::next(cursor);
          break;
        default:
          throw std::runtime_error(std::string("Unknow data type ") + std::to_string(*cursor));
      }
      return *this;
    }

    document_t::iterator_t document_t::Begin() const
    {
      return document_t::iterator_t(this->self, this->offset);
    }

    document_t::iterator_t document_t::End() const
    {
      return document_t::iterator_t(this->self, this->self->end());
    }

    document_t::document_t()
    :self(std::make_shared<dataArray_t>())
    {
      this->self->push_back('\x00');
      this->offset = this->self->begin();
      this->size = 1;
    }

    document_t::document_t(storage_t doc, dataArray_t::const_iterator offset, size_t size)
    :self(doc),
     offset(offset),
     size(size)
    {
      ;
    }

    document_t::document_t(const document_t& cp)
    :self(std::make_shared<dataArray_t>(*cp.self)),
     offset(cp.offset),
     size(cp.size)
    {
      this->offset = this->self->begin() + (cp.offset - cp.self->begin());
    }

    document_t::document_t(document_t&& mv)
    :self(std::move(mv.self)),
     offset(std::move(mv.offset)),
     size(mv.size)
    {
      ;
    }

    document_t& document_t::operator=(const document_t& cp)
    {
      if (this != &cp)
      {
        this->self = std::make_shared<dataArray_t>(*cp.self);
        this->offset = this->self->begin() + (cp.offset - cp.self->begin());
        this->size = cp.size;
      }
      return *this;
    }

    document_t& document_t::operator=(document_t&& mv)
    {
      if (this != &mv)
      {
        this->self = std::move(mv.self);
        this->offset = std::move(mv.offset);
        this->size = mv.size;
      }
      return *this;
    }

    std::string document_t::iterator_t::Key() const
    {
      dataArray_t::const_iterator cursor = this->offset;
      if (*cursor == '\x00')
      {
        throw std::runtime_error("Can't dereference iterator");
      }
      
      ++cursor; // skip type byte
      
      std::string result;
      while(*cursor != '\x00')
      {
        result.push_back(*cursor++);
      }
      return result;
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

      if (newSelfData.back() != '\x00')
      {
        throw std::runtime_error("Invalid BSON document format");
      }
      this->self = std::make_shared<dataArray_t>(std::move(newSelfData));
      this->offset = this->self->begin();
      this->size = this->self->size();
    }

    void document_t::Store(const std::string& filename) const
    {
      if (this->offset != this->self->begin())
      {
        throw std::runtime_error("Only top level document can be stored to file");
      }

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
