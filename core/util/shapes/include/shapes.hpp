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

  class plane_t final
  {
    vao_t vao;

    plane_t(const plane_t&) = delete; 
    plane_t& operator=(const plane_t&) = delete; 

  public:

    void Render();

    plane_t(glm::vec2 size);
    plane_t(plane_t&&) = default; 
    plane_t& operator=(plane_t&&) = default; 
    ~plane_t() = default;
  };

} // namespace bb

#endif /* __BB_UTIL_SHAPES_HEADER__ */