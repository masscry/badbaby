#include <meshDesc.hpp>
#include <context.hpp>

#include <cstring>

namespace bb
{

  size_t basicIndexBuffer_t::TypeSize() const
  {
    return bb::TypeSize(this->Type());
  }

  size_t basicIndexBuffer_t::ByteSize() const
  {
    return this->Size()*this->TypeSize();
  }

  basicIndexBuffer_t::~basicIndexBuffer_t()
  {
    ;
  }

  size_t defaultIndexBuffer_t::Size() const
  {
    return this->size;
  }

  GLenum defaultIndexBuffer_t::Type() const
  {
    return this->type;
  }

  const void* defaultIndexBuffer_t::Data() const
  {
    return this->data.get();
  }

  basicIndexBuffer_t* defaultIndexBuffer_t::Copy() const
  {
    return new defaultIndexBuffer_t(*this);
  }

  defaultIndexBuffer_t::defaultIndexBuffer_t()
  : size(0),
    type(GL_UNSIGNED_SHORT)
  {
    ;
  }

  defaultIndexBuffer_t::defaultIndexBuffer_t(const void* data, size_t size, GLenum type)
  : size(size),
    type(type)
  {
    this->data.reset(new uint8_t[this->ByteSize()]);
    memcpy(this->data.get(), data, this->ByteSize());
  }

  defaultIndexBuffer_t::defaultIndexBuffer_t(const defaultIndexBuffer_t& src)
  : size(src.size),
    type(src.type)
  {
    if (this->ByteSize() > 0)
    {
      this->data.reset(new uint8_t[this->ByteSize()]);
      memcpy(this->data.get(), src.Data(), this->ByteSize());
    }
  }

  defaultIndexBuffer_t::defaultIndexBuffer_t(defaultIndexBuffer_t&& src)
  : data(std::move(src.data)),
    size(src.size),
    type(src.type)
  {
    src.size = 0;
    src.type = GL_UNSIGNED_SHORT;
  }
  
  defaultIndexBuffer_t& defaultIndexBuffer_t::operator=(const defaultIndexBuffer_t& src)
  {
    if (this == &src)
    {
      return *this;
    }

    this->Assign(src);
    return *this;
  }

  defaultIndexBuffer_t& defaultIndexBuffer_t::operator=(defaultIndexBuffer_t&& src)
  {
    if (this == &src)
    {
      return *this;
    }

    this->data = std::move(src.data);
    this->size = src.size;
    this->type = src.type;

    src.size = 0;
    src.type = GL_UNSIGNED_SHORT;
    return *this;
  }
  
  defaultIndexBuffer_t::~defaultIndexBuffer_t()
  {
    ;
  }

  int defaultIndexBuffer_t::Assign(const basicIndexBuffer_t& src)
  {
    if ((src.ByteSize() == 0) || (src.Data() == nullptr))
    {
      BB_PANIC();
      return -1;
    }

    size_t byteSize = src.ByteSize();
    std::unique_ptr<uint8_t[]> newData(new uint8_t[byteSize]);

    memcpy(newData.get(), src.Data(), byteSize);

    this->data = std::move(newData);
    this->size = src.Size();
    this->type = src.Type();
    return 0;
  }

  template<typename data_t>
  size_t MaximumIndexArray(const data_t* data, size_t dataSize)
  {
    data_t result = 0;
    auto breakIndex = bb::breakingIndex<data_t>();

    while(dataSize-->0)
    {
      result = ((result < *data) && (*data != breakIndex))?(*data):(result);
    }
    return static_cast<size_t>(result);
  }

  size_t defaultIndexBuffer_t::MaximumIndex() const
  {
    switch(this->Type())
    {
      case GL_BYTE:
        return MaximumIndexArray(reinterpret_cast<const int8_t*>(this->Data()), this->Size());
      case GL_UNSIGNED_BYTE:
        return MaximumIndexArray(reinterpret_cast<const uint8_t*>(this->Data()), this->Size());
      case GL_SHORT:
        return MaximumIndexArray(reinterpret_cast<const int16_t*>(this->Data()), this->Size());
      case GL_UNSIGNED_SHORT:
        return MaximumIndexArray(reinterpret_cast<const uint16_t*>(this->Data()), this->Size());
      case GL_INT:
        return MaximumIndexArray(reinterpret_cast<const int32_t*>(this->Data()), this->Size());
      case GL_UNSIGNED_INT:
        return MaximumIndexArray(reinterpret_cast<const uint32_t*>(this->Data()), this->Size());
      default:
        BB_PANIC();
        bb::Error("Unknown index array type (0x%x)", this->Type());
        return 0;
    }
  }

  template<typename data_t>
  int CopyPlusOffset(data_t* dst, const data_t* src, size_t size, size_t offset)
  {
    auto breakIndex = bb::breakingIndex<data_t>();
    auto realOffset = static_cast<data_t>(offset);

    if (static_cast<size_t>(realOffset) != offset)
    { // some programmer's mistake
      BB_PANIC();
      return -1;
    }
    if ((dst == nullptr) || (src == nullptr))
    {
      return -1;
    }
    while(size-->0)
    {
      auto srcVal = *src++;
      *dst++ = (srcVal == breakIndex)?(srcVal):(srcVal + realOffset);
    }
    return 0;
  }

  int defaultIndexBuffer_t::Append(const basicIndexBuffer_t& src, size_t offset)
  {
    if (!this->data)
    { // if no data, just assing
      return this->Assign(src);
    }

    if (this->Type() != src.Type())
    { // some programmer's mistake
      bb::Error("%s", "Trying to append buffers of different types");
      BB_PANIC();
      return -1;
    }

    size_t fullByteSize = src.ByteSize() + this->ByteSize();
    std::unique_ptr<uint8_t[]> fullNewData(new uint8_t[fullByteSize]);

    memcpy(fullNewData.get(), this->Data(), this->ByteSize());

    switch(this->Type())
    {
      case GL_BYTE:
        CopyPlusOffset(
          reinterpret_cast<int8_t*>(fullNewData.get() + this->ByteSize()),
          reinterpret_cast<const int8_t*>(src.Data()),
          src.Size(),
          offset
        );
        break;
      case GL_UNSIGNED_BYTE:
        CopyPlusOffset(
          reinterpret_cast<uint8_t*>(fullNewData.get() + this->ByteSize()),
          reinterpret_cast<const uint8_t*>(src.Data()),
          src.Size(),
          offset
        );
        break;
      case GL_SHORT:
        CopyPlusOffset(
          reinterpret_cast<int16_t*>(fullNewData.get() + this->ByteSize()),
          reinterpret_cast<const int16_t*>(src.Data()),
          src.Size(),
          offset
        );
        break;
      case GL_UNSIGNED_SHORT:
        CopyPlusOffset(
          reinterpret_cast<uint16_t*>(fullNewData.get() + this->ByteSize()),
          reinterpret_cast<const uint16_t*>(src.Data()),
          src.Size(),
          offset
        );
        break;
      case GL_INT:
        CopyPlusOffset(
          reinterpret_cast<int32_t*>(fullNewData.get() + this->ByteSize()),
          reinterpret_cast<const int32_t*>(src.Data()),
          src.Size(),
          offset
        );
        break;
      case GL_UNSIGNED_INT:
        CopyPlusOffset(
          reinterpret_cast<uint32_t*>(fullNewData.get() + this->ByteSize()),
          reinterpret_cast<const uint32_t*>(src.Data()),
          src.Size(),
          offset
        );
        break;
      default:
        BB_PANIC();
        bb::Error("Unknown index array type (0x%x)", this->Type());
        return -1;
    }

    memcpy(fullNewData.get() + this->ByteSize(), src.Data(), src.ByteSize());

    this->data = std::move(fullNewData);
    this->size = fullByteSize;
    return 0;
  }

  template<> GLenum indexBuffer_t<uint8_t>::Type() const
  {
    return GL_UNSIGNED_BYTE;
  }

  template<> GLenum indexBuffer_t<int8_t>::Type() const
  {
    return GL_BYTE;
  }

  template<> GLenum indexBuffer_t<uint16_t>::Type() const
  {
    return GL_UNSIGNED_SHORT;
  }

  template<> GLenum indexBuffer_t<int16_t>::Type() const
  {
    return GL_SHORT;
  }

  template<> GLenum indexBuffer_t<uint32_t>::Type() const
  {
    return GL_UNSIGNED_INT;
  }

  template<> GLenum indexBuffer_t<int32_t>::Type() const
  {
    return GL_INT;
  }

}