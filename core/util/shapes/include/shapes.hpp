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

    void Render();

    mesh_t();

    mesh_t(vao_t&& vao, size_t totalVerts, GLenum drawMode, GLuint activeBuffers);

    mesh_t(mesh_t&&) = default; 
    mesh_t& operator=(mesh_t&&) = default; 
    ~mesh_t() = default;
  };

  mesh_t GeneratePlane(glm::vec2 size, glm::vec3 pos);

  mesh_t GeneratePlaneStack(glm::vec2 size, int stackDepth, float startZ, float endZ);

  mesh_t GenerateCircle(int sides, float radius, float width);

} // namespace bb

#endif /* __BB_UTIL_SHAPES_HEADER__ */