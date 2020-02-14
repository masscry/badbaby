#include <shapes.hpp>
#include <common.hpp>

#include <cstdio>
#include <cmath>
#include <utility>
#include <memory>

#include <glm/glm.hpp>

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

    for (auto i = 0u; i < this->activeBuffers; ++i)
    {
      glEnableVertexAttribArray(i);
    }
    glDrawElements(
      this->drawMode,
      static_cast<GLsizei>(this->totalVerts),
      GL_UNSIGNED_SHORT, 0
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
        glm::vec3(vPos[0]*size, zPos),
        glm::vec3(vPos[1]*size, zPos),
        glm::vec3(vPos[2]*size, zPos),
        glm::vec3(vPos[3]*size, zPos)
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

  bb::meshDesc_t DefineCircle(glm::vec3 center, uint32_t sides, float radius, float width)
  {
    bb::vertexBuffer_t<glm::vec2> points;
    bb::vertexBuffer_t<float> distance;
    bb::arrayOfIndecies_t indecies;

    points.SetNormalized(GL_FALSE);
    distance.SetNormalized(GL_FALSE);

    sides = bb::CheckValueBounds(sides, static_cast<uint32_t>(3), static_cast<uint32_t>(std::numeric_limits<uint16_t>::max()/2));
    radius = bb::CheckValueBounds(radius, 0.0f, 1000.0f);
    width = bb::CheckValueBounds(width, 0.001f, radius/2.0f);

    if (sides*4 > std::numeric_limits<uint16_t>::max())
    { // can't be so many sides!
      assert(0);
      sides = std::numeric_limits<uint16_t>::max()/4;
    }

    distance.Self().reserve(sides*2);
    points.Self().reserve(sides*2);
    indecies.reserve(sides*2+2);

    float angle = 0.0f;
    const float angleStep = static_cast<float>(M_PI*2.0/sides);
    const float outerRing = radius + width/2.0f;
    const float innerRing = radius - width/2.0f;
    uint16_t index = 0;

    while(sides-->0)
    {
      glm::vec2 point;
      sincosf(angle, &point.x, &point.y);
      point.x += center.x;
      point.y += center.y;

      points.Self().emplace_back(point * outerRing);
      points.Self().emplace_back(point * innerRing);

      distance.Self().emplace_back(0.0f);
      distance.Self().emplace_back(1.0f);

      indecies.push_back(index++);
      indecies.push_back(index++);
      angle += angleStep;
    }
    indecies.push_back(0);
    indecies.push_back(1);

    bb::meshDesc_t result;

    result.Buffers().emplace_back(
      new decltype(points)(std::move(points))
    );
    result.Buffers().emplace_back(
      new decltype(distance)(std::move(distance))
    );
    result.Indecies() = std::move(indecies);
    result.SetDrawMode(GL_TRIANGLE_STRIP);

    return result;
  }

  bb::mesh_t GenerateCircle(uint32_t sides, float radius, float width)
  {
    return GenerateMesh(DefineCircle(glm::vec3(0.0f), sides, radius, width));
  }

  bb::mesh_t GenerateLine(float width, const linePoints_t& linePoints)
  {
    std::vector<glm::vec2> points;
    std::vector<float> distance;
    std::vector<uint16_t> indecies;

    width = bb::CheckValueBounds(width, 0.01f, 100.0f);

    if ((linePoints.size() < 2) || (linePoints.size()*2 > std::numeric_limits<uint16_t>::max()))
    { // Programmer's error!
      // Can't be so few or so many points
      assert(0);
      return bb::mesh_t();
    }

    points.reserve(linePoints.size()*2);
    distance.reserve(linePoints.size()*2);
    indecies.reserve(linePoints.size()*3);

    uint16_t index = 0;
    for (auto item = linePoints.begin(), end = linePoints.end(); std::next(item) != end; ++item)
    {
      auto nextItem = std::next(item);

      auto dir = glm::normalize(*nextItem - *item);
      auto tangent = glm::vec2(dir.y, -dir.x)*width/2.0f;

      points.push_back(*item + tangent);
      points.push_back(*item - tangent);
      points.push_back(*nextItem + tangent);
      points.push_back(*nextItem - tangent);

      distance.emplace_back(0.0f);
      distance.emplace_back(1.0f);
      distance.emplace_back(0.0f);
      distance.emplace_back(1.0f);

      indecies.push_back(index++);
      indecies.push_back(index++);
      indecies.push_back(index++);
      indecies.push_back(index++);
      indecies.push_back(0xFFFF);
    }

    auto arrayBuffer = bb::vbo_t::CreateArrayBuffer(points, false);
    auto distBuffer = bb::vbo_t::CreateArrayBuffer(distance, false);
    auto elementsBuffer = bb::vbo_t::CreateElementArrayBuffer(indecies, false);
    auto line = bb::vao_t::CreateVertexAttribObject();

    line.BindVBO(arrayBuffer, 0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    line.BindVBO(distBuffer, 1, 1, GL_FLOAT, GL_FALSE, 0, 0);
    line.BindIndecies(elementsBuffer);

    return bb::mesh_t(std::move(line), indecies.size(), GL_TRIANGLE_STRIP, 2);
  }

  mesh_t GenerateMesh(const meshDesc_t& meshDesc)
  {
    if (!meshDesc.IsGood())
    {
      bb::Error("%s", "GenerateMesh from bad description!");
      assert(0);
      return bb::mesh_t();
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
    }

    auto indexVBO = bb::vbo_t::CreateElementArrayBuffer(
      meshDesc.Indecies().data(),
      meshDesc.Indecies().size()*sizeof(uint16_t),
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

  template<>
  GLint vertexBuffer_t<float>::Dimensions() const
  {
    return 1;
  }

  basicVertexBuffer_t::~basicVertexBuffer_t()
  {
    ;
  }

}
