#include <camera.hpp>

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
    glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    return uniformBlock_t(result);
  }

} // namespace bb
