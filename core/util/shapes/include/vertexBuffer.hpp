/**
 * @file vertexBuffer.hpp
 * 
 * Vertex buffer data abstarction
 * 
 */

#pragma once
#ifndef __BB_CORE_UTIL_VERTEX_BUFFER_HEADER__
#define __BB_CORE_UTIL_VERTEX_BUFFER_HEADER__

#include <deque>
#include <vector>
#include <memory>
#include <cassert>

#include <common.hpp>
#include <camera.hpp>

namespace bb
{

  class basicVertexBuffer_t
  {
  public:

    virtual int Append(const basicVertexBuffer_t& src) = 0;
    virtual size_t Size() const = 0;
    virtual GLint Dimensions() const = 0;
    virtual GLenum Type() const = 0;
    virtual GLboolean Normalized() const = 0;
    virtual const void* Data() const = 0;
    virtual ~basicVertexBuffer_t() = 0;
    virtual basicVertexBuffer_t* Copy() const = 0;

    size_t TypeSize() const;
    size_t ByteSize() const;

  };

  template<typename data_t>
  class vertexBuffer_t final: public basicVertexBuffer_t
  {
    using array_t = std::vector<data_t>;

    array_t self;
    GLboolean isNormalized;

  public:

    vertexBuffer_t(const array_t& copy);
    vertexBuffer_t(array_t&& src);

    vertexBuffer_t(const vertexBuffer_t<data_t>&) = default;
    vertexBuffer_t& operator=(const vertexBuffer_t<data_t>&) = default;

    vertexBuffer_t(vertexBuffer_t<data_t>&&) = default;
    vertexBuffer_t& operator=(vertexBuffer_t<data_t>&&) = default;

    vertexBuffer_t();

    array_t& Self();
    const array_t& Self() const;

    void SetNormalized(GLboolean value);

    int Append(const basicVertexBuffer_t& src) override;
    size_t Size() const override;
    GLint Dimensions() const override;
    GLenum Type() const override;
    GLboolean Normalized() const override;
    const void* Data() const override;
    basicVertexBuffer_t* Copy() const override;

    ~vertexBuffer_t() override;
  };

  template<typename data_t>
  std::unique_ptr<basicVertexBuffer_t> MakeVertexBuffer(std::vector<data_t>&& src)
  {
    return std::unique_ptr<basicVertexBuffer_t>(new vertexBuffer_t<data_t>(std::move(src)));
  }

  template<typename data_t>
  vertexBuffer_t<data_t>::vertexBuffer_t()
  : isNormalized(GL_FALSE)
  {
    ;
  }

  template<typename data_t>
  vertexBuffer_t<data_t>::vertexBuffer_t(const array_t& copy)
  : self(copy),
    isNormalized(GL_FALSE)
  {
    ;
  }

  template<typename data_t>
  vertexBuffer_t<data_t>::vertexBuffer_t(array_t&& src)
  : self(std::move(src)),
    isNormalized(GL_FALSE)
  {
    ;
  }

  template<typename data_t>
  vertexBuffer_t<data_t>::~vertexBuffer_t()
  {
    ;
  }

  template<typename data_t>
  inline typename vertexBuffer_t<data_t>::array_t& vertexBuffer_t<data_t>::Self()
  {
    return this->self;
  }

  template<typename data_t>
  inline const typename vertexBuffer_t<data_t>::array_t& vertexBuffer_t<data_t>::Self() const
  {
    return this->self;
  }

  template<typename data_t>
  size_t vertexBuffer_t<data_t>::Size() const
  {
    return this->self.size();
  }

  template<>
  GLint vertexBuffer_t<float>::Dimensions() const;

  template<typename data_t>
  GLint vertexBuffer_t<data_t>::Dimensions() const
  {
    static_assert(
      std::is_same<typename data_t::value_type, float>::value,
      "data_t must contain floats"
    );

    return sizeof(data_t)/sizeof(float);
  }

  template<typename data_t>
  GLenum vertexBuffer_t<data_t>::Type() const
  {
    return GL_FLOAT;
  }

  template<typename data_t>
  GLboolean vertexBuffer_t<data_t>::Normalized() const
  {
    return this->isNormalized;
  }

  template<typename data_t>
  inline void vertexBuffer_t<data_t>::SetNormalized(GLboolean value)
  {
    this->isNormalized = value;
  }

  template<typename data_t>
  const void* vertexBuffer_t<data_t>::Data() const
  {
    return this->self.data();
  }

  template<typename data_t>
  basicVertexBuffer_t* vertexBuffer_t<data_t>::Copy() const
  {
    return new vertexBuffer_t<data_t>(*this);
  }


  template<typename data_t>
  int vertexBuffer_t<data_t>::Append(const basicVertexBuffer_t& src)
  {
    if (this == &src)
    {
      // Programmer's error!
      bb::Error("Can't append buffer to itself");
      assert(0);
      return -1;
    }

    if (
         (src.Dimensions() != this->Dimensions())
      || (src.Type() != this->Type())
      || (src.Normalized() != this->Normalized())
    )
    {
      // Programmer's error!
      bb::Error("Trying to append vertex buffer of different kind");
      assert(0);
      return -1;
    }

    auto itemsToAppend = reinterpret_cast<const data_t*>(src.Data());
    size_t itemsSize = src.Size();

    this->self.insert(this->self.end(), itemsToAppend, itemsToAppend + itemsSize);
    return 0;
  }

  template<typename array_t>
  typename array_t::value_type MaximumIndex(const array_t& array)
  {
    auto result = std::numeric_limits<typename array_t::value_type>::lowest();
    for (auto item: array)
    { // BREAKING_INDEX - special index to break triangle strips,
      // so ignore it
      result = ((result < item) && (item != BREAKING_INDEX))?item:result;
    }
    return result;
  }

} // namespace bb

#endif /* __BB_CORE_UTIL_VERTEX_BUFFER_HEADER__ */