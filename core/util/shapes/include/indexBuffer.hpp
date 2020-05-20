/**
 * @file indexBuffer.hpp
 * 
 * Index buffer data abstraction
 * 
 */

#pragma once
#ifndef __BB_CORE_UTIL_SHAPES_INDEX_BUFFER_HEADER__
#define __BB_CORE_UTIL_SHAPES_INDEX_BUFFER_HEADER__

#include <deque>
#include <vector>
#include <memory>
#include <cassert>

#include <common.hpp>
#include <camera.hpp>

namespace bb
{

  class basicIndexBuffer_t
  {
  public:

    virtual int Append(const basicIndexBuffer_t& src, size_t offset) = 0;
    virtual size_t Size() const = 0;
    virtual GLenum Type() const = 0;
    virtual const void* Data() const = 0;
    virtual ~basicIndexBuffer_t() = 0;
    virtual basicIndexBuffer_t* Copy() const = 0;
    virtual size_t MaximumIndex() const = 0;

    size_t TypeSize() const;
    size_t ByteSize() const;

  };

  class defaultIndexBuffer_t final: public basicIndexBuffer_t
  {
    std::unique_ptr<uint8_t[]> data;
    size_t                     size;
    GLenum                     type;

    int Assign(const basicIndexBuffer_t& src);

  public:

    int Append(const basicIndexBuffer_t& src, size_t offset) override;
    size_t Size() const override;
    GLenum Type() const override;
    const void* Data() const override;
    basicIndexBuffer_t* Copy() const override;
    size_t MaximumIndex() const override;

    defaultIndexBuffer_t();
    defaultIndexBuffer_t(const void* data, size_t size, GLenum type);
    defaultIndexBuffer_t(const defaultIndexBuffer_t&);
    defaultIndexBuffer_t(defaultIndexBuffer_t&&);
    defaultIndexBuffer_t& operator=(const defaultIndexBuffer_t&);
    defaultIndexBuffer_t& operator=(defaultIndexBuffer_t&&);
    ~defaultIndexBuffer_t() override;
  };

  template<typename data_t>
  class indexBuffer_t final: public basicIndexBuffer_t
  {
    using array_t = std::vector<data_t>;

    array_t self;
  public:

    indexBuffer_t(const array_t& copy);
    indexBuffer_t(array_t&& src);

    indexBuffer_t(const indexBuffer_t<data_t>&) = default;
    indexBuffer_t& operator=(const indexBuffer_t<data_t>&) = default;

    indexBuffer_t(indexBuffer_t<data_t>&&) = default;
    indexBuffer_t& operator=(indexBuffer_t<data_t>&&) = default;

    indexBuffer_t();

    array_t& Self();
    const array_t& Self() const;

    int Append(const basicIndexBuffer_t& src, size_t offset) override;
    size_t Size() const override;
    GLenum Type() const override;
    const void* Data() const override;
    basicIndexBuffer_t* Copy() const override;

    size_t MaximumIndex() const override;

    ~indexBuffer_t() override;
  };

  template<typename data_t>
  std::unique_ptr<basicIndexBuffer_t> MakeIndexBuffer(std::vector<data_t>&& src)
  {
    return std::unique_ptr<basicIndexBuffer_t>(new indexBuffer_t<data_t>(std::move(src)));
  }

  template<typename data_t>
  indexBuffer_t<data_t>::indexBuffer_t()
  {
    ;
  }

  template<typename data_t>
  indexBuffer_t<data_t>::indexBuffer_t(const array_t& copy)
  : self(copy)
  {
    ;
  }

  template<typename data_t>
  indexBuffer_t<data_t>::indexBuffer_t(array_t&& src)
  : self(std::move(src))
  {
    ;
  }

  template<typename data_t>
  indexBuffer_t<data_t>::~indexBuffer_t()
  {
    ;
  }

  template<typename data_t>
  inline typename indexBuffer_t<data_t>::array_t& indexBuffer_t<data_t>::Self()
  {
    return this->self;
  }

  template<typename data_t>
  inline const typename indexBuffer_t<data_t>::array_t& indexBuffer_t<data_t>::Self() const
  {
    return this->self;
  }

  template<typename data_t>
  size_t indexBuffer_t<data_t>::Size() const
  {
    return this->self.size();
  }

  template<typename data_t>
  int indexBuffer_t<data_t>::Append(const basicIndexBuffer_t& src, size_t offset)
  {
    if (this == &src)
    {
      // programmer's mistake
      bb::Error("Can't append buffer to itself");
      assert(0);
      return -1;
    }

    if (this->Type() != src.Type())
    {
      // Programmer's error!
      bb::Error("Trying to append index buffer of different kind");
      assert(0);
      return -1;
    }

    auto itemsToAppend = reinterpret_cast<const data_t*>(src.Data());
    size_t itemSize = src.Size();

    this->self.reserve(this->self.size() + itemSize);
    auto breakIndex = bb::breakingIndex<data_t>();
    for (auto cursor = itemsToAppend, end = itemsToAppend + itemSize; cursor != end; ++cursor)
    {
      this->self.emplace_back(
        (cursor[0] != breakIndex)?(cursor[0] + offset):(breakIndex)
      );
    }
    return 0;
  }

  template<> GLenum indexBuffer_t<uint8_t>::Type() const;
  template<> GLenum indexBuffer_t<int8_t>::Type() const;
  template<> GLenum indexBuffer_t<uint16_t>::Type() const;
  template<> GLenum indexBuffer_t<int16_t>::Type() const;
  template<> GLenum indexBuffer_t<uint32_t>::Type() const;
  template<> GLenum indexBuffer_t<int32_t>::Type() const;

  template<typename data_t>
  const void* indexBuffer_t<data_t>::Data() const
  {
    return this->self.data();
  }

  template<typename data_t>
  basicIndexBuffer_t* indexBuffer_t<data_t>::Copy() const
  {
    return new indexBuffer_t<data_t>(*this);
  }

  template<typename data_t>
  size_t indexBuffer_t<data_t>::MaximumIndex() const
  {
    size_t result = 0;
    auto breakIndex = bb::breakingIndex<data_t>();
    for(auto item: this->self)
    {
      result = ((result < item) && (item != breakIndex))?(item):(result);
    }
    return result;
  }

} // namespace bb







#endif /* __BB_CORE_UTIL_SHAPES_INDEX_BUFFER_HEADER__ */