#include <cassert>

#include <texture.hpp>
#include <image.hpp>
#include <config.hpp>
#include <common.hpp>

namespace 
{

  GLint FilterString(const std::string& filterName)
  {
    if (filterName.compare("nearest") == 0)
    {
      return GL_NEAREST;
    }
    if (filterName.compare("linear") == 0)
    {
      return GL_LINEAR;
    }
    if (filterName.compare("linear-mipmap-nearest") == 0)
    {
      return GL_LINEAR_MIPMAP_NEAREST;
    }
    if (filterName.compare("linear-mipmap-linear") == 0)
    {
      return GL_LINEAR_MIPMAP_LINEAR;
    }

    // nearest by default
    bb::Debug("Unknown filter: \"%s\" defaults to GL_NEAREST", filterName.c_str());
    return GL_NEAREST;
  }
  
} // namespace 


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
    assert((data != nullptr) && (width != 0) && (height != 0));

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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  texture_t::texture_t(int width, int height, const float* data)
  {
    assert((data != nullptr) && (width != 0) && (height != 0));

    glGenTextures(1, &this->self);
    glBindTexture(GL_TEXTURE_2D, this->self);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  void texture_t::SetFilter(GLint minFiler, GLint magFilter)
  {
    assert(this->self != 0);
    glBindTexture(GL_TEXTURE_2D, this->self);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFiler);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  void texture_t::GenerateMipmaps()
  {
    assert(this->self != 0);
    glBindTexture(GL_TEXTURE_2D, this->self);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  void texture_t::Bind(const texture_t& tex)
  {
    glBindTexture(GL_TEXTURE_2D, tex.self);
  }

  texture_t texture_t::LoadTGA(const std::string& filename)
  {
    image_t img = bb::LoadTGA(filename);
    return texture_t(img.Width(), img.Height(), img.Depth(), img.Data());
  }

  texture_t texture_t::LoadConfig(const config_t& config)
  {
    std::string imgFile = config.Value("texture.image", "");
    if (imgFile.empty())
    {
      throw std::runtime_error("Texture image filename not found");
    }

    texture_t result = texture_t::LoadTGA(imgFile);

    std::string minFilter = config.Value("texture.min", "nearest");
    std::string magFilter = config.Value("texture.mag", "nearest");

    result.SetFilter(FilterString(minFilter), FilterString(magFilter));

    if (config.Value("texture.mipmaps", 0.0) != 0.0)
    {
      result.GenerateMipmaps();
    }
    return result;
  }

  texture_t texture_t::LoadConfig(const std::string& filename)
  {
    config_t config;
    config.Load(filename);
    return texture_t::LoadConfig(config);
  }

} // namespace bb
