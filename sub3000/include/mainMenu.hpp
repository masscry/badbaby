/**
 * @file mainMenu.h
 * @author main menu scene
 */

#pragma once
#ifndef __SUB3000_MAIN_MENU_HEADER__
#define __SUB3000_MAIN_MENU_HEADER__

#include <scene.hpp>
#include <string>

#include <context.hpp>
#include <font.hpp>
#include <text.hpp>
#include <camera.hpp>

namespace sub3000
{

  class mainMenuScene_t final: public scene_t
  {
    bb::context_t* pContext;
    std::string shader_vp;
    std::string shader_fp;
    std::string fontConfig;

    bb::camera_t      camera;
    bb::shader_t      shader;
    bb::font_t        font;
    bb::textDynamic_t text;

    uint32_t          camBindBlock;

    void OnPrepare() override;
    void OnUpdate(double delta) override;
    void OnRender() override;
    void OnCleanup() override;

  public:

    mainMenuScene_t();
    ~mainMenuScene_t() override;

  };

  
} // namespace sub3000

#endif /* __SUB3000_MAIN_MENU_HEADER__ */