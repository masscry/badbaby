#include <ubo.hpp>

namespace bb
{

  uniformBlock_t::uniformBlock_t(GLuint self)
  :self(self)
  {
    ;
  }

  uniformBlock_t::uniformBlock_t(uniformBlock_t&& mv)
  :self(mv.self)
  {
    mv.self = 0;
  }

  uniformBlock_t& uniformBlock_t::operator=(uniformBlock_t&& mv)
  {
    if (this == &mv)
    {
      return *this;
    }

    if (this->self != 0)
    {
      glDeleteBuffers(1, &this->self);
    }

    this->self = mv.self;
    mv.self = 0;

    return *this;
  }

  uniformBlock_t::uniformBlock_t()
  : self(0)
  {
    ;
  }

  uniformBlock_t::~uniformBlock_t()
  {
    if (this->self != 0)
    {
      glDeleteBuffers(1, &this->self);
    }
  }

  uniformBlock_t uniformBlock_t::CreateUniformBlock(size_t size)
  {
    GLuint result;
    glGenBuffers(1, &result);
    glBindBuffer(GL_UNIFORM_BUFFER, result);
    glBufferData(GL_UNIFORM_BUFFER, static_cast<GLsizeiptr>(size), nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    return uniformBlock_t(result);
  }

  void uniformBlock_t::UpdateData(void* data, int offset, size_t size)
  {
    glBindBuffer(GL_UNIFORM_BUFFER, this->self);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, static_cast<GLsizeiptr>(size), data);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
  }

} // namespace bb
