#include <shapes.hpp>
#include <common.hpp>

#include <cstdio>
#include <cmath>
#include <utility>
#include <memory>
#include <limits>
#include <cinttypes>

#include <glm/glm.hpp>

namespace bb
{

  void mesh_t::Render()
  {
    bb::vao_t::Bind(this->vao);

    if ((this->breakIndex & MF_BREAK) != 0)
    {
      glEnable(GL_PRIMITIVE_RESTART);
      glPrimitiveRestartIndex(this->breakIndex);
    }

    for (auto i = 0u; i < this->activeBuffers; ++i)
    {
      glEnableVertexAttribArray(i);
    }
    glDrawElements(
      this->drawMode,
      static_cast<GLsizei>(this->totalVerts),
      GL_UNSIGNED_SHORT,
      nullptr
    );
    for (auto i = 0u; i < this->activeBuffers; ++i)
    {
      glDisableVertexAttribArray(i);
    }

    if ((this->breakIndex & MF_BREAK) != 0)
    {
      glDisable(GL_PRIMITIVE_RESTART);
    }
  }

  mesh_t::mesh_t()
  : totalVerts(0),
    drawMode(GL_TRIANGLES),
    activeBuffers(2),
    flags(MF_NONE),
    breakIndex(0)
  {
    ;
  }

  mesh_t::mesh_t(vao_t&& vao, size_t totalVerts, GLenum drawMode, GLuint activeBuffers)
  : vao(std::move(vao)),
    totalVerts(totalVerts),
    drawMode(drawMode),
    activeBuffers(activeBuffers),
    flags(MF_NONE),
    breakIndex(0)
  {
    ;
  }

  void mesh_t::Breaking(int32_t enable, uint32_t index)
  {
    this->flags = (this->flags & ~MF_BREAK) | (-enable & MF_BREAK);
    this->breakIndex = index;
  }

  mesh_t GenerateMesh(const meshDesc_t& meshDesc)
  {
    if (!meshDesc.IsGood())
    {
      bb::Error("%s", "GenerateMesh from bad description!");
      assert(0);
      return bb::mesh_t();
    }

    auto maxIndex = meshDesc.Indecies()->MaximumIndex();

    for (auto& dataBuffer: meshDesc.Buffers())
    {
      if (dataBuffer->Size() < maxIndex)
      {
        bb::Error("Data buffer smaller than available indecies (%lu < " BBsize_t ")", dataBuffer->Size(), maxIndex);
        assert(0);
        return bb::mesh_t();
      }
    }

    auto meshVAO = bb::vao_t::CreateVertexAttribObject();

    GLuint arrayBufferIndex = 0;
    for (auto& arrayBuffer: meshDesc.Buffers())
    {
      auto meshVBO = bb::vbo_t::CreateArrayBuffer(
        arrayBuffer->Data(),
        arrayBuffer->ByteSize(),
        false
      );
      meshVAO.BindVBO(
        meshVBO,
        arrayBufferIndex,
        arrayBuffer->Dimensions(),
        arrayBuffer->Type(),
        arrayBuffer->Normalized(),
        0, 0
      );
      ++arrayBufferIndex;
    }

    auto indexVBO = bb::vbo_t::CreateElementArrayBuffer(
      meshDesc.Indecies()->Data(),
      meshDesc.Indecies()->ByteSize(),
      false
    );

    meshVAO.BindIndecies(indexVBO);

    auto mesh = bb::mesh_t(
      std::move(meshVAO),
      meshDesc.Indecies()->Size(),
      meshDesc.DrawMode(),
      static_cast<GLuint>(meshDesc.Buffers().size())
    );

    switch(meshDesc.DrawMode())
    {
      case GL_LINE_STRIP:
      case GL_TRIANGLE_STRIP:
      case GL_TRIANGLE_FAN:
        mesh.Breaking(true, bb::breakingIndex<uint16_t>());
        break;
      default:
        mesh.Breaking(false, 0);
    }

    return mesh;
  }

}
