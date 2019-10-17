/**
 * @file vao.hpp
 * 
 * Vertex Array Object wrapper
 * 
 */

#pragma once
#ifndef __BB_RENDER_VAO_HEADER__
#define __BB_RENDER_VAO_HEADER__

#include <cstddef>

#include <glad/glad.h>

namespace bb
{

  class vbo_t final
  {

    friend class vao_t;

    GLuint self;

    vbo_t(const vbo_t&) = delete;
    vbo_t& operator =(const vbo_t&) = delete;

    vbo_t(GLuint self);

  public:

    vbo_t(vbo_t&&);
    vbo_t& operator =(vbo_t&&);

    vbo_t();
    ~vbo_t();

    static vbo_t CreateArrayBuffer(const void* data, size_t dataSize);

  };

  class vao_t final
  {
    GLuint self;

    vao_t(const vao_t&) = delete;
    vao_t& operator = (const vao_t&) = delete;

    vao_t(GLuint self);

  public:

    vao_t(vao_t&&);
    vao_t& operator = (vao_t&&);

    vao_t();
    ~vao_t();

    void BindVBO(const vbo_t& vbo, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLsizei offset);

    static vao_t CreateVertexAttribObject();

    static void Bind(const vao_t& vao);

  };

} // namespace bb

#endif /* __BB_RENDER_VAO_HEADER__ */