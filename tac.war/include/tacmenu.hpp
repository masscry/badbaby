#pragma once
#ifndef TACWAR_MENU_HEADER
#define TACWAR_MENU_HEADER

#include <tacwar.hpp>

namespace tac
{

  class menu_t final: public scene_t 
  {
    bb::camera_t camera;
    bb::shader_t lineShader;

    bb::mesh_t title;
    std::deque<bb::mesh_t> lines;
    std::deque<glm::vec4> lineCols;
    std::deque<glm::vec2> boundBox;

    void OnClick() override;
    void OnUpdate(double dt) override;
    void OnRender() override;
    void OnAction(int action) override;

    void Prepare();
    void Cleanup();

  public:

    enum lines_t
    {
      newGame = 0,
      exit,
      total
    };

    menu_t()
    : scene_t(tac::state_t::menu)
    {
      this->Prepare();
    }

    ~menu_t()
    {
      this->Cleanup();
    }

  };

}

#endif /* TACWAR_MENU_HEADER */