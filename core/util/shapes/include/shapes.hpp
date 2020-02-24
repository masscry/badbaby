/**
 * @file shapes.hpp
 *
 * Simple 3D shape library
 *
 */

#pragma once
#ifndef __BB_UTIL_SHAPES_HEADER__
#define __BB_UTIL_SHAPES_HEADER__

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <vao.hpp>

#include <deque>
#include <vector>

#include <meshDesc.hpp>

namespace bb
{

  class mesh_t final
  {
    vao_t vao;
    size_t totalVerts;
    GLenum drawMode;
    GLuint activeBuffers;

    mesh_t(const mesh_t&) = delete;
    mesh_t& operator=(const mesh_t&) = delete;

  public:

    bool Good() const;

    void Render();

    mesh_t();

    mesh_t(vao_t&& vao, size_t totalVerts, GLenum drawMode, GLuint activeBuffers);

    mesh_t(mesh_t&&) = default;
    mesh_t& operator=(mesh_t&&) = default;
    ~mesh_t() = default;
  };

  inline bool mesh_t::Good() const
  {
    return (this->totalVerts != 0) && (this->vao.Good());
  }

  mesh_t GeneratePlane(glm::vec2 size, glm::vec3 pos);

  mesh_t GeneratePlaneStack(glm::vec2 size, uint32_t stackDepth, float startZ, float endZ);

  mesh_t GenerateCircle(uint32_t sides, float radius, float width);

  mesh_t GenerateLine(float width, const linePoints_t& linePoints);

  mesh_t GenerateMesh(const meshDesc_t& meshDesc);

} // namespace bb

#endif /* __BB_UTIL_SHAPES_HEADER__ */
