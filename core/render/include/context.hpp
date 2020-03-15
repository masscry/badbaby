/**
 * @file context.hpp
 * 
 * OpenGL context initialization
 * 
 */

#pragma once
#ifndef __BB_CORE_RENDER_CONTEXT_HEADER__
#define __BB_CORE_RENDER_CONTEXT_HEADER__

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

#include <list>
#include <utility>

#include <framebuffer.hpp>
#include <shader.hpp>
#include <vao.hpp>

namespace bb
{

  enum contextMsgFlag_t: uint32_t 
  {
    cmfNone     = 0x0000,
    cmfKeyboard = 0x0001
  };

  using actorPID_t = int64_t;

  class context_t final
  {
    static bool isAlreadyExists;

    using pairOfFlags = std::pair<actorPID_t, contextMsgFlag_t>;
    using actorCallbackList_t = std::list<pairOfFlags>;

    GLFWwindow*         wnd;
    int                 width;
    int                 height;
    framebuffer_t       canvas;
    shader_t            shader;
    vao_t               vao;
    actorCallbackList_t actorCallbackList;
    bool                insideWnd;
    bool                relativeCursor;

    context_t();
    ~context_t();

    context_t(const context_t&) = delete;
    context_t(context_t&&) = delete;

    context_t& operator=(const context_t&) = delete;
    context_t& operator=(context_t&&) = delete;

    static void OnCursorEnter(GLFWwindow* window, int entered);
    static void OnKey(GLFWwindow* window, int key, int scancode, int action, int mods);

  public:

    framebuffer_t& Canvas();

    int Width() const;
    int Height() const;
    float AspectRatio() const;

    glm::vec2 Dimensions() const;

    static context_t& Instance();
    static bool IsAlreadyExists();

    bool Update();

    bool IsKeyDown(uint16_t key) const;
    glm::dvec2 MousePos() const;

    void RelativeCursor(bool enable);
    bool IsCursorInside() const;

    void Title(const std::string& newTitle);

    void RegisterActorCallback(actorPID_t actorID, contextMsgFlag_t flags);
    void UnregisterActorCallbacks(actorPID_t actorID);

    static void UnregisterActorCallbacksIfContextExists(actorPID_t actorID)
    {
      if (context_t::IsAlreadyExists())
      {
        context_t::Instance().UnregisterActorCallbacks(actorID);
      }
    }

  };

  inline glm::vec2 context_t::Dimensions() const
  {
    return glm::vec2(
      static_cast<float>(this->Width()),
      static_cast<float>(this->Height())
    );
  }

  inline float context_t::AspectRatio() const
  {
    return static_cast<float>(this->Width())/static_cast<float>(this->Height());
  }

  inline framebuffer_t& context_t::Canvas()
  {
    return this->canvas;
  }

  inline int context_t::Width() const 
  {
    return this->width;
  }

  inline int context_t::Height() const 
  {
    return this->height;
  }

  inline bool context_t::IsKeyDown(uint16_t key) const
  {
    return glfwGetKey(this->wnd, key) != GLFW_RELEASE;
  }

  inline glm::dvec2 context_t::MousePos() const
  {
    glm::dvec2 result;
    glfwGetCursorPos(this->wnd, &result.x, &result.y);
    return result;
  }

  inline bool context_t::IsCursorInside() const
  {
    return this->insideWnd;
  }

} // namespace bb

#endif /* __BB_CORE_RENDER_CONTEXT_HEADER__ */
