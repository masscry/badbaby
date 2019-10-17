/**
 * @file shader.hpp
 * 
 * GLSL shader class
 * 
 */
#pragma once
#ifndef __BB_CORE_RENDER_SHADER_HEADER__
#define __BB_CORE_RENDER_SHADER_HEADER__

#include <glad/glad.h>

namespace bb
{
  
  class shader_t final
  {
    GLuint handle;

    shader_t(const shader_t&) = delete;
    shader_t& operator=(const shader_t& move) = delete;

  public:

    shader_t(shader_t&& move);
    shader_t& operator=(shader_t&& move);

    shader_t();
    shader_t(const char* vpSource, const char* fpSource);
    ~shader_t();

    static void Bind(const shader_t& shader);

  };

} // namespace bb

#endif /* __BB_CORE_RENDER_SHADER_HEADER__ */