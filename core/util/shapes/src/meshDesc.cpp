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

} // namespace bb
