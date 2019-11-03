/**
 * @file texture.hpp
 * 
 * Simple texture wrapper class
 * 
 */
#pragma once
#ifndef __BB_CORE_RENDER_TEXTURE_HEADER__
#define __BB_CORE_RENDER_TEXTURE_HEADER__

#include <glad/glad.h>

#include <memory>

#include <config.hpp>

namespace bb
{
  
  class texture_t final
  {
    // friend let framebuffer to have access to texture_t::self to bind it
    friend class framebuffer_t;
    GLuint self;

    texture_t(const texture_t&) = delete;
    texture_t& operator=(const texture_t&) = delete;

  public:

    void SetFilter(GLint minFiler, GLint magFilter);
    void GenerateMipmaps();

    texture_t(texture_t&&);
    texture_t& operator=(texture_t&&);

    texture_t();
    texture_t(int width, int height);
    texture_t(int width, int height, int depth, const uint8_t* data);
    texture_t(int width, int height, const float* data);

    ~texture_t();

    static texture_t LoadConfig(const config_t& config);
    static texture_t LoadConfig(const std::string& filename);
    static texture_t LoadTGA(const std::string& filename);
    static void Bind(const texture_t& tex);

  };

  using sharedTexture_t = std::shared_ptr<texture_t>;

} // namespace bb

#endif /* __BB_CORE_RENDER_TEXTURE_HEADER__ */