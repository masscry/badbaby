/**
 * @file authors.h
 * @author authors page
 */

#pragma once
#ifndef __SUB3000_AUTHORS_HEADER__
#define __SUB3000_AUTHORS_HEADER__

#include <string>

#include <scene.hpp>

#include <context.hpp>
#include <font.hpp>
#include <text.hpp>
#include <camera.hpp>
#include <actor.hpp>
#include <role.hpp>
#include <mailbox.hpp>

#include <sub3000.hpp>

namespace sub3000
{

  class authorsModel_t final: public bb::role_t
  {
    bb::msgResult_t OnProcessMessage(const bb::actor_t&, bb::msg_t msg) override;
  public:

    const char* DefaultName() const override
    {
      return "authors";
    }

    authorsModel_t();
  };

  class authorsScene_t final: public scene_t
  {
    bb::context_t* pContext;
    std::string shader_vp;
    std::string shader_fp;
    std::string fontConfig;
    bb::vec2_t  textSize;
    size_t      maxLineWidth;

    bb::camera_t      camera;
    bb::shader_t      shader;
    bb::font_t        font;

    uint32_t          camBindBlock;
    int               modelBindPoint;
    int               glyphColorBindPoint;

    std::string      authorsFilename;
    bb::textStatic_t authorsText;
    bb::node_t       authorsNode;

    int model;

    void OnPrepare() override;
    void OnUpdate(double delta) override;
    void OnRender() override;
    void OnCleanup() override;

  public:

    authorsScene_t();
    ~authorsScene_t() override;

  };

  
} // namespace sub3000

#endif /* __SUB3000_MAIN_MENU_HEADER__ */
