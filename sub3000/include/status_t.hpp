/**
 * @file status_t.hpp
 * 
 * Status text
 * 
 */
#pragma once
#ifndef __SUB3000_ARENA_STATUS_HEADER__
#define __SUB3000_ARENA_STATUS_HEADER__

#include <scene.hpp>
#include <vao.hpp>
#include <shader.hpp>
#include <framebuffer.hpp>
#include <camera.hpp>
#include <mailbox.hpp>
#include <text.hpp>

namespace sub3000
{

  namespace radar
  {

    class status_t final: public scene_t
    {
      bb::framebuffer_t fb;
      bb::shader_t shader;
      bb::camera_t camera;
      bb::mailbox_t::shared_t box;

      bb::font_t font;
      bb::textDynamic_t text;

      status_t(const status_t&) = delete;
      status_t& operator=(const status_t&) = delete;

      void OnPrepare() override;
      void OnUpdate(double delta) override;
      void OnRender() override;
      void OnCleanup() override;

    public:

      bb::framebuffer_t& Framebuffer();
      const bb::framebuffer_t& Framebuffer() const;

      status_t(status_t&&) = default;
      status_t& operator=(status_t&&) = default;
      ~status_t() override = default;

      status_t();
      static status_t Create();

    };

  } // namespace radar

} // namespace sub3000

#endif /* __SUB3000_ARENA_STATUS_HEADER__ */