/**
 * @file text.hpp
 * 
 * Simple Text Class
 * 
 */
#pragma once
#ifndef __BB_CORE_RENDER_TEXT_HEADER__
#define __BB_CORE_RENDER_TEXT_HEADER__

#include <common.hpp>
#include <font.hpp>
#include <shapes.hpp>

namespace bb
{

  class textStatic_t final
  {
    sharedTexture_t tex;
    mesh_t mesh;

    textStatic_t(const textStatic_t&) = delete;
    textStatic_t& operator=(const textStatic_t&) = delete;

  public:

    void Render();

    textStatic_t();
    textStatic_t(const font_t& font, const std::string& text, vec2_t chSize, size_t maxWidth);

    textStatic_t(textStatic_t&&) = default;
    textStatic_t& operator=(textStatic_t &&) = default;

    ~textStatic_t() = default;
  };

  template<typename T>
  using storage_t = std::vector<T>;

  struct textStorage_t
  {
    storage_t<vec3_t>   vPos;
    storage_t<vec2_t>   vUV;
    storage_t<vec4_t>   vCol;
    storage_t<uint16_t> indecies;
  };

  template<typename T>
  size_t ByteSize(const storage_t<T>& arr)
  {
    return arr.size()*sizeof(T);
  }

  class textDynamic_t final
  {
    vao_t vao;
    const font_t* font;
    vec2_t chSize;

    vbo_t vPosVBO;
    vbo_t vUVVBO;
    vbo_t vColVBO;
    vbo_t indeciesVBO;
    size_t totalV;  /**< total allocated vertecies */ 
    size_t totalI;  /**< total allocated indecies  */
    size_t renderI; /**< total indecies to render  */

    textStorage_t vertecies;

    textDynamic_t(const textDynamic_t&) = delete;
    textDynamic_t& operator=(const textDynamic_t&) = delete;

    void UpdateText(const char* text);

  public:

    void Update(const std::string& text);

    void Update(const char* format, ...) BB_FORMAT_LIKE_PRINTF(2, 3);

    void Render();

    textDynamic_t();

    textDynamic_t(const font_t& font, const vec2_t& chSize);

    textDynamic_t(textDynamic_t&&) = default;
    textDynamic_t& operator=(textDynamic_t&&) = default;
    ~textDynamic_t() = default;
  };






} // namespace bb

#endif /* __BB_CORE_RENDER_TEXT_HEADER__ */