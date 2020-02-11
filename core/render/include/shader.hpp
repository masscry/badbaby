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
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
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

    void SetVector2f(GLint loc, const glm::vec2& value) const;
    void SetVector3f(GLint loc, const glm::vec3& value) const;
    void SetVector4f(GLint loc, const glm::vec4& value) const;

    void SetFloat(const char* name, float value) const;
    void SetVector2f(const char* name, GLsizei count, const float* values) const;
    void SetVector3f(const char* name, GLsizei count, const float* values) const;
    void SetVector4f(const char* name, GLsizei count, const float* values) const;
    void SetMatrix(const char* name, const float* matrix) const;
    void SetTexture(const char* name, int texUnit) const;

    void SetVector2f(const char* name, const glm::vec2& value) const;
    void SetVector3f(const char* name, const glm::vec3& values) const;
    void SetVector4f(const char* name, const glm::vec4& values) const;
    void SetMatrix(const char* name, const glm::mat4& matrix) const;

    GLuint UniformBlockIndex(const char* name) const;
    void SetBlock(GLuint blockIndex, const uniformBlock_t& block);

    shader_t();
    shader_t(const char* vpSource, const char* fpSource);
    ~shader_t();

    static void Bind(const shader_t& shader);

    static shader_t LoadProgramFromFiles(const char* vpFilename, const char* fpFilename);

  };

  inline void shader_t::SetFloat(const char* name, float value) const
  {
    this->SetFloat(this->UniformLocation(name), value);
  }

  inline void shader_t::SetVector2f(const char* name, GLsizei count, const float* values) const
  {
    this->SetVector2f(this->UniformLocation(name), count, values);
  }

  inline void shader_t::SetVector3f(const char* name, GLsizei count, const float* values) const
  {
    this->SetVector3f(this->UniformLocation(name), count, values);
  }

  inline void shader_t::SetVector4f(const char* name, GLsizei count, const float* values) const
  {
    this->SetVector4f(this->UniformLocation(name), count, values);
  }

  inline void shader_t::SetMatrix(const char* name, const float* matrix) const
  {
    this->SetMatrix(this->UniformLocation(name), matrix);
  }

  inline void shader_t::SetTexture(const char* name, int texUnit) const
  {
    this->SetTexture(this->UniformLocation(name), texUnit);
  }

  inline void shader_t::SetVector2f(const char* name, const glm::vec2& value) const
  {
    this->SetVector2f(this->UniformLocation(name), value);
  }

  inline void shader_t::SetVector3f(const char* name, const glm::vec3& value) const
  {
    this->SetVector3f(this->UniformLocation(name), value);
  }

  inline void shader_t::SetVector4f(const char* name, const glm::vec4& value) const
  {
    this->SetVector4f(this->UniformLocation(name), value);
  }

  inline void shader_t::SetMatrix(const char* name, const glm::mat4& matrix) const
  {
    this->SetMatrix(
      this->UniformLocation(name),
      &matrix[0][0]
    );
  }

} // namespace bb

#endif /* __BB_CORE_RENDER_SHADER_HEADER__ */
