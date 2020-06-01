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

  size_t TypeSize(GLenum type);

  using actorPID_t = int64_t;

  class context_t final
  {
  public:
    
    enum msgFlag_t : uint32_t
    {
      none = 0x0000,
      keyboard = 0x0001
    };

  private:

    static bool isAlreadyExists;

    using pairOfFlags = std::pair<actorPID_t, msgFlag_t>;
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

    std::mutex          mutex;
    std::string         title;
    bool                hasNewTitle;

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

    void SetStickyMouse(bool enable) const;

    glm::dvec2 MousePos() const;
    bool IsButtonPressed(int button) const;

    void RelativeCursor(bool enable);
    bool IsCursorInside() const;

    void Title(const std::string& newTitle);

    void RegisterActorCallback(actorPID_t actorID, context_t::msgFlag_t flags);
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

  inline bool context_t::IsButtonPressed(int button) const
  {
    return glfwGetMouseButton(this->wnd, button) != GLFW_RELEASE;
  }

  inline void context_t::SetStickyMouse(bool enable) const
  {
    glfwSetInputMode(
      this->wnd,
      GLFW_STICKY_MOUSE_BUTTONS,
      (enable)?(GLFW_TRUE):(GLFW_FALSE)
    );
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
