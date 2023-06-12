#include <meshDesc.hpp>
#include <context.hpp>

#include <cstring>

namespace bb
{

  size_t basicVertexBuffer_t::TypeSize() const
  {
    return bb::TypeSize(this->Type());
  }

  size_t basicVertexBuffer_t::ByteSize() const
  {
    return this->Size()*static_cast<size_t>(this->Dimensions())*this->TypeSize();
  }

  basicVertexBuffer_t::~basicVertexBuffer_t()
  {
    ;
  }

  size_t defaultVertexBuffer_t::Size() const
  {
    return this->size;
  }

  GLint defaultVertexBuffer_t::Dimensions() const
  {
    return this->dim;
  }

  GLenum defaultVertexBuffer_t::Type() const
  {
    return this->type;
  }

  GLboolean defaultVertexBuffer_t::Normalized() const
  {
    return this->normalized;
  }

  const void* defaultVertexBuffer_t::Data() const
  {
    return this->data.get();
  }

  basicVertexBuffer_t* defaultVertexBuffer_t::Copy() const
  {
    return new defaultVertexBuffer_t(*this);
  }

  int defaultVertexBuffer_t::Assign(const basicVertexBuffer_t& src)
  {
    if ((src.ByteSize() == 0) || (src.Data() == nullptr))
    { // programmer's mistake
      BB_PANIC();
      return -1;
    }

    size_t byteSize = src.ByteSize();
    std::unique_ptr<uint8_t[]> newData(new uint8_t[byteSize]);

    memcpy(newData.get(), src.Data(), byteSize);

    this->data       = std::move(newData);
    this->size       = src.Size();
    this->dim        = src.Dimensions();
    this->type       = src.Type();
    this->normalized = src.Normalized();
    return 0;
  }

  int defaultVertexBuffer_t::Append(const basicVertexBuffer_t& src)
  {
    if (!this->data)
    { // if no data, just assign
      return this->Assign(src);
    }

    if (
         (this->Dimensions() != src.Dimensions())
      || (this->Type() != src.Type())
      || (this->Normalized() != src.Normalized())
    )
    { // some programmer's mistake
      bb::Error("%s", "Trying to append buffers of different types");
      BB_PANIC();
      return -1;
    }

    // otherwise append data at end
    size_t fullByteSize = src.ByteSize() + this->ByteSize();
    size_t fullItemSize = src.Size() + this->Size();

    std::unique_ptr<uint8_t[]> fullNewData(new uint8_t[fullByteSize]);

    memcpy(fullNewData.get(), this->Data(), this->ByteSize());
    memcpy(fullNewData.get() + this->ByteSize(), src.Data(), src.ByteSize());

    this->data = std::move(fullNewData);
    this->size = fullItemSize;
    return 0;
  }

  defaultVertexBuffer_t::defaultVertexBuffer_t()
  : size(0),
    dim(0),
    type(GL_FLOAT),
    normalized(GL_FALSE)
  {
    ;
  }

  defaultVertexBuffer_t::defaultVertexBuffer_t(
    const void* data,
    size_t size,
    GLint dim,
    GLenum type,
    GLboolean normalized
  )
  : size(size),
    dim(dim),
    type(type),
    normalized(normalized)
  {
    this->data.reset(new uint8_t[this->ByteSize()]);
    memcpy(this->data.get(), data, this->ByteSize());
  }

  defaultVertexBuffer_t::defaultVertexBuffer_t(
    const defaultVertexBuffer_t& src
  )
  : size(src.size),
    dim(src.dim),
    type(src.type),
    normalized(src.normalized)
  {
    if (this->ByteSize() != 0)
    {
      this->data.reset(new uint8_t[this->ByteSize()]);
      memcpy(this->data.get(), src.Data(), this->ByteSize());
    }
  }

  defaultVertexBuffer_t::defaultVertexBuffer_t(
    defaultVertexBuffer_t&& src
  )
  : data(std::move(src.data)),
    size(src.size),
    dim(src.dim),
    type(src.type),
    normalized(src.normalized)
  {
    src.size = 0;
    src.dim  = 0;
    src.type = GL_FLOAT;
    src.normalized = GL_FALSE;
  }

  defaultVertexBuffer_t& defaultVertexBuffer_t::operator = (const defaultVertexBuffer_t& src)
  {
    if (this == &src)
    {
      return *this;
    }

    this->Assign(src);
    return *this;
  }

  defaultVertexBuffer_t& defaultVertexBuffer_t::operator = (defaultVertexBuffer_t&& src)
  {
    if (this == &src)
    {
      return *this;
    }

    this->data       = std::move(src.data);
    this->size       = src.size;
    this->dim        = src.dim;
    this->type       = src.type;
    this->normalized = src.normalized;

    src.size = 0;
    src.dim  = 0;
    src.type = GL_FLOAT;
    src.normalized = GL_FALSE;
    return *this;
  }

  defaultVertexBuffer_t::~defaultVertexBuffer_t()
  {
    ;
  }

  template<>
  GLint vertexBuffer_t<float>::Dimensions() const
  {
    return 1;
  }

} // namespace bb
