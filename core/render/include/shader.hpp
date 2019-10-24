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

#include <ubo.hpp>

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

    GLint UniformLocation(const char* name) const;

    void SetFloat(GLint loc, float value) const;
    void SetVector2f(GLint loc, GLsizei count, const float* values) const;
    void SetVector3f(GLint loc, GLsizei count, const float* values) const;
    void SetVector4f(GLint loc, GLsizei count, const float* values) const;
    void SetMatrix(GLint loc, const float* matrix) const;
    void SetTexture(GLint loc, int texUnit) const;

    GLuint UniformBlockIndex(const char* name) const;

    void SetBlock(GLuint blockIndex, const uniformBlock_t& block);

    shader_t();
    shader_t(const char* vpSource, const char* fpSource);
    ~shader_t();

    static void Bind(const shader_t& shader);

    static shader_t LoadProgramFromFiles(const char* vpFilename, const char* fpFilename);

  };

} // namespace bb

#endif /* __BB_CORE_RENDER_SHADER_HEADER__ */