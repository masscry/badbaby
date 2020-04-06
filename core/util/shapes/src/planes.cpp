#include <shapes.hpp>
#include <common.hpp>

#include <cstdio>
#include <cstring>
#include <cmath>
#include <utility>
#include <memory>
#include <limits>

#include <glm/glm.hpp>

namespace
{

  const glm::vec2 vPos[4] = {
    { 0.0f, 0.0f },
    { 1.0f, 0.0f },
    { 1.0f, 1.0f },
    { 0.0f, 1.0f }
  };

  const glm::vec2 vUV[4] = {
    { 0.0f, 1.0f },
    { 1.0f, 1.0f },
    { 1.0f, 0.0f },
    { 0.0f, 0.0f }
  };

  const glm::vec2 vYFlipUV[4] = {
    { 0.0f, 0.0f },
    { 1.0f, 0.0f },
    { 1.0f, 1.0f },
    { 0.0f, 1.0f }
  };

  static_assert(sizeof(vUV) == sizeof(vYFlipUV),
    "this arrays are used interchangably"
  );

  const uint16_t vInd[6] = {
    0, 1, 2, 0, 2, 3
  };

}

namespace bb
{

  mesh_t GeneratePlane(glm::vec2 size, glm::vec3 pos, glm::vec2 origin, bool flipY)
  {
    glm::vec3 xPos[4] = {
      glm::vec3((vPos[0]-origin)*size, 0.0f),
      glm::vec3((vPos[1]-origin)*size, 0.0f),
      glm::vec3((vPos[2]-origin)*size, 0.0f),
      glm::vec3((vPos[3]-origin)*size, 0.0f)
    };

    xPos[0] += pos;
    xPos[1] += pos;
    xPos[2] += pos;
    xPos[3] += pos;


    auto vboPos = bb::vbo_t::CreateArrayBuffer(xPos, sizeof(xPos), false);
    auto vboInd = bb::vbo_t::CreateElementArrayBuffer(vInd, sizeof(vInd), false);
    auto vao    = bb::vao_t::CreateVertexAttribObject();

    bb::vbo_t vboUV = (flipY)?
      (bb::vbo_t::CreateArrayBuffer(vYFlipUV, sizeof(vYFlipUV), false))
      :(bb::vbo_t::CreateArrayBuffer(vUV, sizeof(vUV), false));

    vao.BindVBO(vboPos, 0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    vao.BindVBO(vboUV,  1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    vao.BindIndecies(vboInd);

    return mesh_t(std::move(vao), 6, GL_TRIANGLES, 2);
  }

  mesh_t GeneratePlaneStack(glm::vec2 size, uint32_t stackDepth, float startZ, float endZ)
  {
    std::unique_ptr<glm::vec3[]> posBuf(new glm::vec3[4*stackDepth]);
    std::unique_ptr<glm::vec2[]> uvBuf(new glm::vec2[4*stackDepth]);
    std::unique_ptr<uint16_t[]> indBuf(new uint16_t[6*stackDepth]);

    float deltaZ = (endZ - startZ)/static_cast<float>(stackDepth-1);

    uint32_t indOffset = 0;
    for (uint32_t i = 0; i < stackDepth; ++i)
    {
      float zPos = static_cast<float>(i)*deltaZ + startZ;

      glm::vec3 xPos[4] = {
        glm::vec3((vPos[0]-vec2_t(0.5f))*size, zPos),
        glm::vec3((vPos[1]-vec2_t(0.5f))*size, zPos),
        glm::vec3((vPos[2]-vec2_t(0.5f))*size, zPos),
        glm::vec3((vPos[3]-vec2_t(0.5f))*size, zPos)
      };

      uint16_t indecies[6] = {
        static_cast<uint16_t>(vInd[0] + indOffset),
        static_cast<uint16_t>(vInd[1] + indOffset),
        static_cast<uint16_t>(vInd[2] + indOffset),
        static_cast<uint16_t>(vInd[3] + indOffset),
        static_cast<uint16_t>(vInd[4] + indOffset),
        static_cast<uint16_t>(vInd[5] + indOffset)
      };

      memcpy(posBuf.get() + i*4,     xPos, sizeof(glm::vec3)*4);
      memcpy(uvBuf.get()  + i*4,      vUV, sizeof(glm::vec2)*4);
      memcpy(indBuf.get() + i*6, indecies, sizeof(uint16_t)*6);

      indOffset += 4u;
    }

    auto vboPos = bb::vbo_t::CreateArrayBuffer(posBuf.get(),        sizeof(glm::vec3)*stackDepth*4u, false);
    auto vboUV  = bb::vbo_t::CreateArrayBuffer(uvBuf.get(),         sizeof(glm::vec2)*stackDepth*4u, false);
    auto vboInd = bb::vbo_t::CreateElementArrayBuffer(indBuf.get(), sizeof(uint16_t)*stackDepth*6u, false);
    auto vao    = bb::vao_t::CreateVertexAttribObject();

    vao.BindVBO(vboPos, 0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    vao.BindVBO(vboUV,  1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    vao.BindIndecies(vboInd);

    return mesh_t(std::move(vao), stackDepth*6, GL_TRIANGLES, 2);
  }

} // namespace bb