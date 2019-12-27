/**
 * @file mainMenu.h
 * @author main menu scene
 */

#pragma once
#ifndef __SUB3000_MAIN_MENU_HEADER__
#define __SUB3000_MAIN_MENU_HEADER__

#include <string>
#include <deque>


#include <scene.hpp>

#include <context.hpp>
#include <font.hpp>
#include <text.hpp>
#include <camera.hpp>
#include <actor.hpp>
#include <mailbox.hpp>

#include <sub3000.hpp>

namespace sub3000
{

  struct textNode_t {
    bb::textDynamic_t text;
    bb::node_t node;
    gameAction_t action;

    textNode_t(bb::textDynamic_t&& text, bb::node_t&& node, gameAction_t action)
    : text(std::move(text)),
      node(std::move(node)),
      action(action)
    {
      ;
    }
  };

  using textList_t = std::deque<textNode_t>;

  class mainMenuModel_t final: public bb::actor_t
  {
    textList_t& textList;
    bb::mailbox_t& view;

    uint32_t selectedLine;

    void ProcessMessage(bb::msg_t msg) override;

  public:

    uint32_t SelectedLine() const
    {
      return this->selectedLine;
    }

    mainMenuModel_t(textList_t& textList, bb::mailbox_t& view);

  };

  class mainMenuScene_t final: public scene_t
  {

    using menuLines_t = std::vector<std::string>;

    bb::context_t* pContext;
    std::string shader_vp;
    std::string shader_fp;
    std::string fontConfig;
    std::string gameInfo;
    bb::vec2_t  textSize;

    menuLines_t textLines; // text to show
    menuLines_t msgLines;  // message to send on activation

    bb::camera_t      camera;
    bb::shader_t      shader;
    bb::font_t        font;

    uint32_t          camBindBlock;
    uint32_t          modelBindPoint;
    uint32_t          glyphColorBindPoint;

    int menuModelID;
    bb::mailbox_t mailbox;
    uint32_t selectedMenuLine;

    textList_t textList;

    bb::textStatic_t gameInfoText;
    bb::node_t       gameInfoNode;

    void OnPrepare() override;
    void OnUpdate(double delta) override;
    void OnRender() override;
    void OnCleanup() override;

    menuLines_t LoadMenuLines(const bb::config_t& config, const std::string& prefix);

  public:

    mainMenuScene_t();
    ~mainMenuScene_t() override;

  };

  
} // namespace sub3000

#endif /* __SUB3000_MAIN_MENU_HEADER__ */