/**
 * @file algebra.hpp
 * 
 * Algebraic definitions
 * 
 */

#pragma once
#ifndef __BB_RENDER_ALGEBRA_HEADER__
#define __BB_RENDER_ALGEBRA_HEADER__

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cmath>

namespace bb
{
  using mat4_t = glm::mat4;
  using vec2_t = glm::vec2;
  using vec3_t = glm::vec3;
  using vec4_t = glm::vec4;

#ifdef _WIN32
  inline vec2_t Dir(float angle)
  {
    return vec2_t(sinf(angle), cosf(angle));
  }
#endif

#ifdef __linux__
  inline vec2_t Dir(float angle)
  {
    vec2_t dir;
    sincosf(angle, &dir.x, &dir.y);
    return dir;
  }
#endif

#ifdef __APPLE__
  inline vec2_t Dir(float angle)
  {
    vec2_t dir;
    __sincosf(angle, &dir.x, &dir.y);
    return dir;
  }
#endif

}

#endif /* __BB_RENDER_ALGEBRA_HEADER__ */
