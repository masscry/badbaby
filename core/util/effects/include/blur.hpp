/**
 * @file blur.hpp
 *
 * Simple gaussian blur effect
 *
 */

#pragma once
#ifndef __BB_UTIL_BLUR_HEADER__
#define __BB_UTIL_BLUR_HEADER__

#include <common.hpp>
#include <camera.hpp>
#include <shapes.hpp>
#include <shader.hpp>
#include <framebuffer.hpp>

namespace bb
{

  class blur_t
  {
    bb::mesh_t plane;
    bb::shader_t shader;
    bb::framebuffer_t temp;
    bb::framebuffer_t* src;
    bb::framebuffer_t* dst;
    int fboSize;

    blur_t(const blur_t&) = delete;
    blur_t& operator=(const blur_t&) = delete;

  public:

    void Render();

    blur_t();
    blur_t(bb::framebuffer_t* src, bb::framebuffer_t* dst, int fboSize);
    ~blur_t();

    blur_t(blur_t&&) = default;
    blur_t& operator=(blur_t&&) = default;

  };

} // namespace bb

#endif /* __BB_UTIL_BLUR_HEADER__ */
