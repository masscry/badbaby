#include <meshDesc.hpp>
#include <cstring>

namespace bb
{

  meshDesc_t::meshDesc_t()
  : drawMode(GL_TRIANGLES)
  {
    ;
  }

  meshDesc_t::~meshDesc_t()
  {
    ;
  }

  size_t meshDesc_t::MaxIndex() const
  {
    return this->indecies->MaximumIndex();
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
      this->indecies.reset(mesh.Indecies()->Copy());
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

    return this->indecies->Append(*mesh.indecies, this->MaxIndex() + 1);
  }

  bool meshDesc_t::IsGood() const
  {
    return (!this->buffers.empty()) && (this->indecies->Size() != 0);
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
      arrayHeader.dim = static_cast<uint32_t>(arrayBuffer->Dimensions()); // dimension can't be < 0
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

    assert(this->Indecies()->Size() < std::numeric_limits<uint32_t>::max());
    assert(this->Indecies()->ByteSize() + sizeof(meshDescArrayHeader_t) < std::numeric_limits<uint32_t>::max());

    meshDescArrayHeader_t arrayHeader;
    arrayHeader.magic = MESH_DESC_ELEMENT_MAGIC;
    arrayHeader.size = static_cast<uint32_t>(this->Indecies()->Size());
    arrayHeader.dim = 1;
    arrayHeader.type = this->Indecies()->Type();
    arrayHeader.normalized = GL_FALSE;
    arrayHeader.byteSize = static_cast<uint32_t>(
      this->Indecies()->ByteSize() + sizeof(meshDescArrayHeader_t)
    );

    if (fwrite(&arrayHeader, sizeof(meshDescArrayHeader_t), 1, output) != 1)
    {
      return -1;
    }
    if (fwrite(this->Indecies()->Data(), this->Indecies()->TypeSize(), this->Indecies()->Size(), output) != this->Indecies()->Size())
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
                 static_cast<GLint>(arrHeader.dim), // OpenGL wants GLint as dimension
                 arrHeader.type,
                 static_cast<GLboolean>(arrHeader.normalized)
               )
             );          
          }
          break;
        case MESH_DESC_ELEMENT_MAGIC:
          {
            size_t elemDataByteSize = arrHeader.byteSize - sizeof(meshDescArrayHeader_t);

            std::unique_ptr<uint8_t[]> elemData(new uint8_t[elemDataByteSize]);
            if (fread(elemData.get(), 1, elemDataByteSize, input) != elemDataByteSize)
            {
              assert(0);
              return meshDesc_t();
            }

            result.indecies.reset(
              new defaultIndexBuffer_t(elemData.get(), arrHeader.size, arrHeader.type)
            );
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
