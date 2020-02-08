/**
 * @file vao.hpp
 * 
 * Vertex Array Object wrapper
 * 
 */

#pragma once
#ifndef __BB_CORE_RENDER_VAO_HEADER__
#define __BB_CORE_RENDER_VAO_HEADER__

#include <cstddef>
#include <vector>

#include <glad/glad.h>

namespace bb
{

  class vbo_t final
  {

    friend class vao_t;

    GLuint self;
    GLenum type;

    vbo_t(const vbo_t&) = delete;
    vbo_t& operator =(const vbo_t&) = delete;

    vbo_t(GLuint self, GLenum type);

  public:

    vbo_t(vbo_t&&);
    vbo_t& operator =(vbo_t&&);

    vbo_t();
    ~vbo_t();

    void Update(int offset, size_t size, const void* data);

    static vbo_t CreateArrayBuffer(const void* data, size_t dataSize, bool dynamic);

    static vbo_t CreateElementArrayBuffer(const void* data, size_t dataSize, bool dynamic);

    template<typename data_t>
    static vbo_t CreateArrayBuffer(const std::vector<data_t>& data, bool dynamic)
    {
      return vbo_t::CreateArrayBuffer(data.data(), sizeof(data_t)*data.size(), dynamic);
    }

    template<typename data_t>
    static vbo_t CreateElementArrayBuffer(const std::vector<data_t>& data, bool dynamic)
    {
      return vbo_t::CreateElementArrayBuffer(data.data(), sizeof(data_t)*data.size(), dynamic);
    }

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

    void BindIndecies(const vbo_t& vbo);

    static vao_t CreateVertexAttribObject();

    static void Bind(const vao_t& vao);
    static void Unbind();

  };

} // namespace bb

#endif /* __BB_CORE_RENDER_VAO_HEADER__ */