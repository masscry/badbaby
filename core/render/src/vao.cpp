#include <cassert>

#include <vao.hpp>

namespace bb
{

  vbo_t::vbo_t(vbo_t &&move) noexcept
  :self(move.self),type(move.type)
  {
    move.self = 0;
    move.type = 0;
  }

  vbo_t &vbo_t::operator=(vbo_t &&move) noexcept
  {
    if (this == &move)
    {
      return *this;
    }

    if (this->self != 0)
    {
      glDeleteBuffers(1, &this->self);
    }

    this->self = move.self;
    this->type = move.type;
    move.self = 0;
    move.type = 0;
    return *this;
  }

  vbo_t::vbo_t()
  :self(0),type(0)
  {
    ;
  }

  vbo_t::~vbo_t()
  {
    if (this->self != 0)
    {
      glDeleteBuffers(1, &this->self);
    }
  }

  vbo_t::vbo_t(GLuint self, GLenum type)
  :self(self),type(type)
  {
    ;
  }

  void vbo_t::Update(int offset, size_t size, const void* data)
  {
    glBindBuffer(this->type, this->self);
    glBufferSubData(this->type, offset, static_cast<GLsizeiptr>(size), data);
    glBindBuffer(this->type, 0);
  }

  vbo_t vbo_t::CreateArrayBuffer(const void* data, size_t dataSize, bool dynamic)
  {
    GLuint vbo;

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(dataSize), data, (dynamic)?GL_DYNAMIC_DRAW:GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return vbo_t(vbo, GL_ARRAY_BUFFER);
  }

  vbo_t vbo_t::CreateElementArrayBuffer(const void* data, size_t dataSize, bool dynamic)
  {
    GLuint vbo;

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(dataSize), data, (dynamic)?GL_DYNAMIC_DRAW:GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return vbo_t(vbo, GL_ELEMENT_ARRAY_BUFFER);
  }

  vao_t::vao_t(vao_t&& move)
  :self(move.self)
  {
    move.self = 0;
  }

  vao_t& vao_t::operator = (vao_t&& move)
  {
    if (this == &move)
    {
      return *this;
    }

    if (this->self != 0)
    {
      glDeleteVertexArrays(1, &this->self);
    }

    this->self = move.self;
    move.self = 0;

    return *this;
  }

  vao_t::vao_t()
  :self(0)
  {
    ;
  }

  vao_t::vao_t(GLuint self)
  :self(self)
  {
    ;
  }

  vao_t::~vao_t()
  {
    if (this->self != 0)
    {
      glDeleteVertexArrays(1, &this->self);
    }
  }

  vao_t vao_t::CreateVertexAttribObject()
  {
    GLuint self;
    glGenVertexArrays(1, &self);
    return vao_t(self);
  }

  void vao_t::BindVBO(const vbo_t& vbo, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLsizei offset)
  {
    void *pOffset = reinterpret_cast<void*>(static_cast<uintptr_t>(offset));

    assert(this->self != 0);
    glBindVertexArray(this->self);
    glBindBuffer(GL_ARRAY_BUFFER, vbo.self);
    glVertexAttribPointer(index, size, type, normalized, stride, pOffset);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

  void vao_t::BindIndecies(const vbo_t& vbo)
  {
    assert(this->self != 0);
    glBindVertexArray(this->self);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo.self);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }

  void vao_t::Bind(const vao_t& vao)
  {
    glBindVertexArray(vao.self);
  }
  
  void vao_t::Unbind()
  {
    glBindVertexArray(0);
  }


} // namespace bb
