#include <cassert>

#include <texture.hpp>
#include <image.hpp>

namespace bb
{

  texture_t::texture_t(texture_t&& move)
  :self(move.self)
  {
    move.self = 0;
  }

  texture_t& texture_t::operator=(texture_t&& move)
  {
    if (this == &move)
    {
      return *this;
    }

    if (this->self != 0)
    {
      glDeleteTextures(1, &this->self);
    }

    this->self = move.self;
    move.self = 0;
    return *this;
  }

  texture_t::texture_t()
  :self(0)
  {
    ;
  }

  texture_t::~texture_t()
  {
    if (this->self != 0)
    {
      glDeleteTextures(1, &this->self);
    }
  }

  texture_t::texture_t(int width, int height)
  :self(0)
  {
    glGenTextures(1, &this->self);
    glBindTexture(GL_TEXTURE_2D, this->self);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  texture_t::texture_t(int width, int height, int depth, const uint8_t* data)
  :self(0)
  {
    assert((depth == 3) || (depth == 4));

    glGenTextures(1, &this->self);
    glBindTexture(GL_TEXTURE_2D, this->self);
    switch (depth)
    {
    case 3:
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
      break;
    case 4:
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
      break;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  void texture_t::Bind(const texture_t& tex)
  {
    glBindTexture(GL_TEXTURE_2D, tex.self);
  }

  texture_t texture_t::LoadTGA(const char* filename)
  {
    image_t img = bb::LoadTGA(filename);
    return texture_t(img.Width(), img.Height(), img.Depth(), img.Data());
  }

} // namespace bb
