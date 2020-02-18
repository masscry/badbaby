#include <meshDesc.hpp>

namespace bb
{

  size_t basicVertexBuffer_t::TypeSize() const
  {
    switch(this->Type())
    {
      case GL_BYTE:
      case GL_UNSIGNED_BYTE:
        return 1;
      case GL_SHORT:
      case GL_UNSIGNED_SHORT:
      case GL_HALF_FLOAT:
        return 2;
      case GL_INT:
      case GL_UNSIGNED_INT:
      case GL_FIXED:
      case GL_FLOAT:
        return 4;
      default:
        // programmer's error
        bb::Error("Unsupported type: (%d)", this->Type());
        assert(0);
        return 0;
    }
  }

  size_t basicVertexBuffer_t::ByteSize() const
  {
    return this->Size()*this->Dimensions()*this->TypeSize();
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
      assert(0);
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
      assert(0);
      return -1;
    }

    // otherwise append data at end
    size_t fullByteSize = src.ByteSize() + this->ByteSize();
    std::unique_ptr<uint8_t[]> fullNewData(new uint8_t[fullByteSize]);

    memcpy(fullNewData.get(), this->Data(), this->ByteSize());
    memcpy(fullNewData.get() + this->ByteSize(), src.Data(), src.ByteSize());

    this->data = std::move(fullNewData);
    this->size = fullByteSize;
    return 0;
  }

  defaultVertexBuffer_t::defaultVertexBuffer_t()
  : size(0),
    dim(0),
    type(GL_TRIANGLES),
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
    src.type = GL_TRIANGLES;
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
    src.type = GL_TRIANGLES;
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

  meshDesc_t::meshDesc_t()
  : drawMode(GL_TRIANGLES)
  {
    ;
  }

  meshDesc_t::~meshDesc_t()
  {
    ;
  }

  uint16_t meshDesc_t::MaxIndex() const
  {
    return MaximumIndex(this->indecies);
  }

  int meshDesc_t::Append(const meshDesc_t& mesh)
  {
    if (this == &mesh)
    {
      bb::Error("%s", "Can't append mesh to itself");
      assert(0);
      return -1;
    }

    if (this->buffers.empty())
    { // destination is empty, just copy
      this->drawMode = mesh.drawMode;
      this->indecies = mesh.Indecies();
      for (auto& buffer: mesh.Buffers())
      {
        this->buffers.emplace_back(buffer->Copy());
      }
      return 0;
    }

    // otherwise buffers expected to be merged

    if (
         (this->drawMode != mesh.drawMode)
      || (this->buffers.size() != mesh.buffers.size())
      )
    {
      // Programmer's error!
      bb::Error("%s", "Can't append mesh of different type");
      assert(0);
      return -1;
    }

    for (size_t bufID = 0, lastBufID = this->buffers.size(); bufID != lastBufID; ++bufID)
    {
      auto& dst = this->buffers[bufID];
      auto& src = mesh.buffers[bufID];
      if (dst->Append(*src) != 0)
      {
        return -1;
      }
    }

    //
    // After new vertecies added at end of dst buffer, we need to fix indecies
    //
    switch (this->DrawMode())
    {
      case GL_LINE_STRIP:
      case GL_TRIANGLE_STRIP: // we adding breaking index for strips
        this->indecies.reserve(this->indecies.size() + mesh.indecies.size() + 1);
        this->indecies.push_back(BREAKING_INDEX);
        break;
      default:
        this->indecies.reserve(this->indecies.size() + mesh.indecies.size());
    }

    uint16_t thisMaxIndex = static_cast<uint16_t>(this->MaxIndex() + 1);
    for (auto index: mesh.indecies)
    { 
      //
      // break-index must stay unchanged
      //
      this->indecies.emplace_back(
        (index != BREAKING_INDEX)?(index + thisMaxIndex):(BREAKING_INDEX)
      );
    }
    return 0;
  }

  bool meshDesc_t::IsGood() const
  {
    return (!this->buffers.empty()) && (!this->indecies.empty());
  }

#pragma pack(push, 1)

  const uint32_t MESH_DESC_MAGIC         = 0x736d6462;
  const uint32_t MESH_DESC_VERSION       = 0x01;
  const uint32_t MESH_DESC_ARRAY_MAGIC   = 0x72616462;
  const uint32_t MESH_DESC_ELEMENT_MAGIC = 0x72656462;

  struct meshDescHeader_t
  {
    uint32_t magic;       // 0x736d6462 // "bdms"
    uint32_t version;     // always 1
    uint32_t dataOffset;  // where data starts
    uint32_t bufferCount; // total buffer count
    uint32_t drawMode;    // draw mode
  };

  struct meshDescArrayHeader_t
  {
    uint32_t magic; // 0x72616462 // "bdar" || or 0x72656462 "bder"
    uint32_t size;
    uint32_t dim;
    uint32_t type;
    uint32_t normalized;
    uint32_t byteSize;
  };

#pragma pack(pop)

  int meshDesc_t::Save(FILE* output) const
  {
    assert((this->Buffers().size() + 1) < std::numeric_limits<uint32_t>::max());

    meshDescHeader_t header;
    header.magic = MESH_DESC_MAGIC;
    header.version = MESH_DESC_VERSION;
    header.dataOffset = sizeof(meshDescHeader_t);
    header.bufferCount = static_cast<uint32_t>(this->Buffers().size() + 1); // arrays + element array
    header.drawMode = this->DrawMode();

    if (fwrite(&header, sizeof(meshDescHeader_t), 1, output) != 1)
    {
      return -1;
    }

    for (auto& arrayBuffer: this->Buffers())
    {
      assert(arrayBuffer->Size() < std::numeric_limits<uint32_t>::max());
      assert(arrayBuffer->ByteSize() + sizeof(meshDescArrayHeader_t) < std::numeric_limits<uint32_t>::max());

      meshDescArrayHeader_t arrayHeader;
      arrayHeader.magic = MESH_DESC_ARRAY_MAGIC;
      arrayHeader.size = static_cast<uint32_t>(arrayBuffer->Size());
      arrayHeader.dim = arrayBuffer->Dimensions();
      arrayHeader.type = arrayBuffer->Type();
      arrayHeader.normalized = arrayBuffer->Normalized();
      arrayHeader.byteSize = static_cast<uint32_t>(arrayBuffer->ByteSize() + sizeof(meshDescArrayHeader_t));

      if (fwrite(&arrayHeader, sizeof(meshDescArrayHeader_t), 1, output) != 1)
      {
        return -1;
      }
      if (fwrite(arrayBuffer->Data(), 1, arrayBuffer->ByteSize(), output) != arrayBuffer->ByteSize())
      {
        return -1;
      }
    }

    assert(this->Indecies().size() < std::numeric_limits<uint32_t>::max());
    assert(this->Indecies().size()*sizeof(uint16_t) + sizeof(meshDescArrayHeader_t) < std::numeric_limits<uint32_t>::max());

    meshDescArrayHeader_t arrayHeader;
    arrayHeader.magic = MESH_DESC_ELEMENT_MAGIC;
    arrayHeader.size = static_cast<uint32_t>(this->Indecies().size());
    arrayHeader.dim = 1;
    arrayHeader.type = GL_UNSIGNED_SHORT;
    arrayHeader.normalized = GL_FALSE;
    arrayHeader.byteSize = static_cast<uint32_t>(
      this->Indecies().size()*sizeof(uint16_t) + sizeof(meshDescArrayHeader_t)
    );

    if (fwrite(&arrayHeader, sizeof(meshDescArrayHeader_t), 1, output) != 1)
    {
      return -1;
    }
    if (fwrite(this->Indecies().data(), sizeof(uint16_t), this->Indecies().size(), output) != this->Indecies().size())
    {
      return -1;
    }
    return 0;
  }

  bool meshDesc_t::CheckFile(FILE* input)
  {
    meshDescHeader_t header;
    auto fpos = ftell(input);
    if (fread(&header, sizeof(meshDescHeader_t), 1, input) != 1)
    {
      return false;
    }
    fseek(input, fpos, SEEK_SET);
    return (
         (header.magic == MESH_DESC_MAGIC)
      || (header.version == MESH_DESC_VERSION)
    );
  }

  meshDesc_t meshDesc_t::Load(FILE* input)
  {
    meshDesc_t result;
    meshDescHeader_t header;

    if (fread(&header, sizeof(meshDescHeader_t), 1, input) != 1)
    {
      assert(0);
      return meshDesc_t();
    }

    if ((header.magic != MESH_DESC_MAGIC) || (header.version != MESH_DESC_VERSION))
    {
      assert(0);
      return meshDesc_t();
    }

    result.SetDrawMode(header.drawMode);

    if (fseek(input, header.dataOffset, SEEK_SET) != 0)
    {
      assert(0);
      return meshDesc_t();
    }

    for (uint32_t bufID = 0; bufID < header.bufferCount; ++bufID)
    {
      meshDescArrayHeader_t arrHeader;

      if (fread(&arrHeader, sizeof(meshDescArrayHeader_t), 1, input) != 1)
      {
        assert(0);
        return meshDesc_t();
      }
     
      switch(arrHeader.magic)
      {
        case MESH_DESC_ARRAY_MAGIC:
          {
            size_t arrayDataByteSize = arrHeader.byteSize - sizeof(meshDescArrayHeader_t);

             std::unique_ptr<uint8_t[]> arrayData(new uint8_t[arrayDataByteSize]);
             if (fread(arrayData.get(), 1, arrayDataByteSize, input) != arrayDataByteSize)
             {
               assert(0);
               return meshDesc_t();
             }

             result.buffers.emplace_back(
               new bb::defaultVertexBuffer_t(
                 arrayData.get(),
                 arrHeader.size,
                 arrHeader.dim,
                 arrHeader.type,
                 static_cast<GLboolean>(arrHeader.normalized)
               )
             );          
          }
          break;
        case MESH_DESC_ELEMENT_MAGIC:        
          {
            result.Indecies().resize(arrHeader.size);
            if (fread(
              result.Indecies().data(),
              sizeof(uint16_t), result.Indecies().size(),
              input) != result.Indecies().size()
            )
            {
              assert(0);
              return meshDesc_t();
            }            
          }
          break;
        default:
          assert(0);
          return meshDesc_t();        
      }
      
    }
        
    return result;
  }

} // namespace bb
