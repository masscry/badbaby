#include <shapes.hpp>
#include <utility>
#include <cstdio>
#include <memory>

namespace
{

  const glm::vec2 vPos[4] = {
    { -0.5f, -0.5f },
    { +0.5f, -0.5f },
    { +0.5f, +0.5f },
    { -0.5f, +0.5f}
  };

  const glm::vec2 vUV[4] = {
    { 0.0f, 1.0f },
    { 1.0f, 1.0f },
    { 1.0f, 0.0f },
    { 0.0f, 0.0f }
  };

  const uint16_t vInd[6] = {
    0, 1, 2, 0, 2, 3
  };

}

namespace bb
{

  void mesh_t::Render()
  {
    bb::vao_t::Bind(this->vao);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glDrawElements(GL_TRIANGLES, this->totalVerts, GL_UNSIGNED_SHORT, 0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
  }

  mesh_t::mesh_t(vao_t&& vao, size_t totalVerts)
  :vao(std::move(vao)),
   totalVerts(totalVerts)
  {

  }

  mesh_t GeneratePlane(glm::vec2 size, glm::vec3 pos)
  {
    glm::vec3 xPos[4] = {
      glm::vec3(vPos[0]*size, 0.0f),
      glm::vec3(vPos[1]*size, 0.0f),
      glm::vec3(vPos[2]*size, 0.0f),
      glm::vec3(vPos[3]*size, 0.0f)
    };

    xPos[0] += pos;
    xPos[1] += pos;
    xPos[2] += pos;
    xPos[3] += pos;

    auto vboPos = bb::vbo_t::CreateArrayBuffer(xPos, sizeof(xPos), false);
    auto vboUV  = bb::vbo_t::CreateArrayBuffer(vUV,  sizeof(vUV),  false);
    auto vboInd = bb::vbo_t::CreateElementArrayBuffer(vInd, sizeof(vInd), false);
    auto vao    = bb::vao_t::CreateVertexAttribObject();

    vao.BindVBO(vboPos, 0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    vao.BindVBO(vboUV,  1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    vao.BindIndecies(vboInd);

    return mesh_t(std::move(vao), 6);
  }

  mesh_t GeneratePlaneStack(glm::vec2 size, int stackDepth, float startZ, float endZ)
  {
    std::unique_ptr<glm::vec3[]> posBuf(new glm::vec3[4*stackDepth]);
    std::unique_ptr<glm::vec2[]> uvBuf(new glm::vec2[4*stackDepth]);
    std::unique_ptr<uint16_t[]> indBuf(new uint16_t[6*stackDepth]);

    float deltaZ = (endZ - startZ)/(stackDepth-1);

    for (int i = 0; i < stackDepth; ++i)
    {
      glm::vec3 xPos[4] = {
        glm::vec3(vPos[0]*size, i*deltaZ + startZ),
        glm::vec3(vPos[1]*size, i*deltaZ + startZ),
        glm::vec3(vPos[2]*size, i*deltaZ + startZ),
        glm::vec3(vPos[3]*size, i*deltaZ + startZ)
      };

      printf("%d %f\n", i, i*deltaZ + startZ);

      uint16_t indecies[6] = {
        (uint16_t)(vInd[0] + (i*4)),
        (uint16_t)(vInd[1] + (i*4)),
        (uint16_t)(vInd[2] + (i*4)),
        (uint16_t)(vInd[3] + (i*4)),
        (uint16_t)(vInd[4] + (i*4)),
        (uint16_t)(vInd[5] + (i*4))
      };

      printf("\t%d %d %d %d %d %d\n",
        indecies[0],
        indecies[1],
        indecies[2],
        indecies[3],
        indecies[4],
        indecies[5]
      );

      memcpy(posBuf.get() + i*4,     xPos, sizeof(glm::vec3)*4);
      memcpy(uvBuf.get()  + i*4,      vUV, sizeof(glm::vec2)*4);
      memcpy(indBuf.get() + i*6, indecies, sizeof(uint16_t)*6);
    }

    auto vboPos = bb::vbo_t::CreateArrayBuffer(posBuf.get(),        sizeof(glm::vec3)*stackDepth*4, false);
    auto vboUV  = bb::vbo_t::CreateArrayBuffer(uvBuf.get(),         sizeof(glm::vec2)*stackDepth*4, false);
    auto vboInd = bb::vbo_t::CreateElementArrayBuffer(indBuf.get(), sizeof(uint16_t)*stackDepth*6, false);
    auto vao    = bb::vao_t::CreateVertexAttribObject();

    vao.BindVBO(vboPos, 0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    vao.BindVBO(vboUV,  1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    vao.BindIndecies(vboInd);

    return mesh_t(std::move(vao), stackDepth*6);
  }


}