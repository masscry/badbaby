#include <meshDesc.hpp>

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

  uint16_t meshDesc_t::MaxIndex() const
  {
    uint16_t maxIndex = 0;
    for (auto index: this->indecies)
    {
      maxIndex = (maxIndex<index)?index:maxIndex;
    }
    return maxIndex;
  }

  int meshDesc_t::Append(const meshDesc_t& mesh)
  {
    if (this == &mesh)
    {
      bb::Error("%s", "Can't append mesh to itself");
      assert(0);
      return -1;
    }

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
    this->indecies.reserve(this->indecies.size() + mesh.indecies.size());
    uint16_t thisMaxIndex = this->MaxIndex();
    for (auto index: mesh.indecies)
    {
      this->indecies.emplace_back(index + thisMaxIndex);
    }
    return 0;
  }

  bool meshDesc_t::IsGood() const
  {
    return (!this->buffers.empty()) && (!this->indecies.empty());
  }


} // namespace bb
