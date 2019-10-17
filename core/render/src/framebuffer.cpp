#include <stdexcept>

#include <framebuffer.hpp>

namespace bb
{
  framebuffer_t::framebuffer_t(framebuffer_t&& move)
  :tex(std::move(move.tex)),self(move.self)
  {
    move.self = 0;
  }

  framebuffer_t& framebuffer_t::operator=(framebuffer_t&& move)
  {
    if (this == &move)
    {
      return *this;
    }

    if (this->self != 0)
    {
      glDeleteFramebuffers(1, &this->self);
      this->tex = std::move(texture_t());
    }

    this->tex  = std::move(move.tex);
    this->self = move.self;
    move.self = 0;

    return *this;
  }

  framebuffer_t::framebuffer_t()
  :self(0)
  {
    ;
  }

  framebuffer_t::framebuffer_t(int width, int height)
  :tex(width, height),self(0)
  {
    GLuint rbo;

    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glGenFramebuffers(1, &this->self);
    glBindFramebuffer(GL_FRAMEBUFFER, this->self);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->tex.self, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
      throw std::runtime_error("Failed to cmplete framebuffer");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glDeleteRenderbuffers(1, &rbo);
  }

  framebuffer_t::~framebuffer_t()
  {
    if (this->self != 0)
    {
      glDeleteFramebuffers(1, &this->self);
    }
  }

  void framebuffer_t::Bind(const framebuffer_t& fb)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, fb.self);
  }

} // namespace bb
