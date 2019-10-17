#include <stdexcept>

#include <framebuffer.hpp>

namespace bb
{
  framebuffer_t::framebuffer_t(framebuffer_t&& move)
  :self(move.self),tex(move.tex),rbo(move.rbo)
  {
    move.self = 0;
    move.tex  = 0;
    move.rbo  = 0;
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
      glDeleteRenderbuffers(1, &this->rbo);
      glDeleteTextures(1, &this->tex);
    }

    this->self = move.self;
    this->tex  = move.tex;
    this->rbo  = move.rbo;
    move.self = 0;
    move.tex  = 0;
    move.rbo  = 0;

    return *this;
  }

  framebuffer_t::framebuffer_t()
  :self(0),tex(0),rbo(0)
  {
    ;
  }

  framebuffer_t::framebuffer_t(int width, int height)
  :self(0),tex(0),rbo(0)
  {
    glGenTextures(1, &this->tex);
    glBindTexture(GL_TEXTURE_2D, this->tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenRenderbuffers(1, &this->rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, this->rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glGenFramebuffers(1, &this->self);
    glBindFramebuffer(GL_FRAMEBUFFER, this->self);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->tex, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
      throw std::runtime_error("Failed to cmplete framebuffer");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  framebuffer_t::~framebuffer_t()
  {
    if (this->self != 0)
    {
      glDeleteFramebuffers(1, &this->self);
      glDeleteRenderbuffers(1, &this->rbo);
      glDeleteTextures(1, &this->tex);
    }
  }

  void framebuffer_t::Bind(const framebuffer_t& fb)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, fb.self);
  }

} // namespace bb
