#include <shapes.hpp>
#include <common.hpp>

#include <cstdio>
#include <cmath>
#include <utility>
#include <memory>
#include <limits>

#include <glm/glm.hpp>

namespace bb
{

  void mesh_t::Render()
  {
    bb::vao_t::Bind(this->vao);

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
  }

  mesh_t::mesh_t()
  : totalVerts(0),
    drawMode(GL_TRIANGLES),
    activeBuffers(2)
  {
    ;
  }

  mesh_t::mesh_t(vao_t&& vao, size_t totalVerts, GLenum drawMode, GLuint activeBuffers)
  : vao(std::move(vao)),
    totalVerts(totalVerts),
    drawMode(drawMode),
    activeBuffers(activeBuffers)
  {
    ;
  }

  mesh_t GenerateMesh(const meshDesc_t& meshDesc)
  {
    if (!meshDesc.IsGood())
    {
      bb::Error("%s", "GenerateMesh from bad description!");
      assert(0);
      return bb::mesh_t();
    }

    auto maxIndex = MaximumIndex(meshDesc.Indecies());

    for (auto& dataBuffer: meshDesc.Buffers())
    {
      if (dataBuffer->Size() < maxIndex)
      {
        bb::Error("Data buffer smaller than available indecies (%lu < %u)", dataBuffer->Size(), maxIndex);
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
      meshDesc.Indecies(),
      false
    );

    meshVAO.BindIndecies(indexVBO);

    return bb::mesh_t(
      std::move(meshVAO),
      meshDesc.Indecies().size(),
      meshDesc.DrawMode(),
      static_cast<GLuint>(meshDesc.Buffers().size())
    );
  }

}
