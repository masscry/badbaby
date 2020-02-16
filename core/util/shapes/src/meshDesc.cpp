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

      if (arrHeader.magic != MESH_DESC_ARRAY_MAGIC)
      {
        assert(0);
        return meshDesc_t();
      }

      



    }

    return result;
  }

} // namespace bb
