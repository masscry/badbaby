/**
 * @file meshDesc.hpp
 * 
 * Mesh description.
 * 
 */

#pragma once
#ifndef __BB_CORE_UTIL_SHAPES_MESH_DESCRIPTOR_HEADER__
#define __BB_CORE_UTIL_SHAPES_MESH_DESCRIPTOR_HEADER__

#include <vertexBuffer.hpp>

namespace bb
{

  using arrayOfIndecies_t = std::vector<uint16_t>;
  using arrayOfVertexBuffers_t = std::deque<std::unique_ptr<basicVertexBuffer_t>>;
  using linePoints_t = std::deque<glm::vec2>;

  class meshDesc_t final
  {
    arrayOfVertexBuffers_t buffers;
    arrayOfIndecies_t indecies;
    GLenum drawMode;

    meshDesc_t(const meshDesc_t&) = delete;
    meshDesc_t& operator=(const meshDesc_t&) = delete;

  public:

    static meshDesc_t Load(FILE* input);

    int Save(FILE* output) const;
    
    arrayOfVertexBuffers_t& Buffers();
    const arrayOfVertexBuffers_t& Buffers() const;

    arrayOfIndecies_t& Indecies();
    const arrayOfIndecies_t& Indecies() const;

    GLenum DrawMode() const;

    void SetDrawMode(GLenum drawMode);

    uint16_t MaxIndex() const;

    bool IsGood() const;

    meshDesc_t();
    ~meshDesc_t();

    meshDesc_t(meshDesc_t&&) = default;
    meshDesc_t& operator=(meshDesc_t&&) = default;

    int Append(const meshDesc_t& mesh);

  };

  inline arrayOfVertexBuffers_t& meshDesc_t::Buffers()
  {
    return this->buffers;
  }

  inline const arrayOfVertexBuffers_t& meshDesc_t::Buffers() const
  {
    return this->buffers;
  }

  inline arrayOfIndecies_t& meshDesc_t::Indecies()
  {
    return this->indecies;
  }

  inline const arrayOfIndecies_t& meshDesc_t::Indecies() const
  {
    return this->indecies;
  }

  inline GLenum meshDesc_t::DrawMode() const
  {
    return this->drawMode;
  }

  inline void meshDesc_t::SetDrawMode(GLenum drawMode)
  {
    this->drawMode = drawMode;
  }

  meshDesc_t DefineCircle(glm::vec3 center, uint32_t sides, float radius, float width);
  meshDesc_t DefineLine(glm::vec3 offset, float width, const linePoints_t& linePoints);

} // namespace bb

#endif /* __BB_CORE_UTIL_SHAPES_MESH_DESCRIPTOR_HEADER__ */
