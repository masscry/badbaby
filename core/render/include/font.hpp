/**
 * @file font.hpp
 * 
 * Simple font rendering.
 */

#pragma once
#ifndef __BB_CORE_RENDER_FONT_HEADER__
#define __BB_CORE_RENDER_FONT_HEADER__

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>
#include <map>

#include <texture.hpp>
#include <vao.hpp>

namespace bb
{

  using vec2_t = glm::vec2;
  using vec3_t = glm::vec3;

  class font_t final
  {
    sharedTexture_t texture;
    std::string encoding;
    int width;
    int height;

    using smbToVec_t = std::map<uint32_t, vec2_t>;
    smbToVec_t offsets;
    smbToVec_t sizes;

    font_t(const font_t&) = delete;
    font_t& operator=(const font_t&) = delete;

  public:

    font_t(const std::string& filename)
    {
      this->Load(filename);
    }

    font_t() = default;
    font_t(font_t&&) = default;
    font_t& operator=(font_t&&) = default;

    ~font_t() = default;

    void Load(const std::string& filename);

    const sharedTexture_t& Texture() const 
    {
      return this->texture;
    }

    int Width() const 
    {
      return this->Width(); 
    }

    int Height() const 
    {
      return this->Height(); 
    }

    vec2_t SymbolOffset(uint32_t smb) const
    {
      auto it = this->offsets.find(smb);
      if (it == this->offsets.end())
      {
        return vec2_t(0.0f, 0.0f);
      }
      return it->second;
    }

    vec2_t SymbolSize(uint32_t smb) const
    {
      auto it = this->sizes.find(smb);
      if (it == this->sizes.end())
      {
        return vec2_t(1.0f, 1.0f);
      }
      return it->second;
    }

  };

} // namespace bb

#endif /* __BB_CORE_RENDER_FONT_HEADER__ */