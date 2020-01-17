#include <cassert>

#include <stdexcept>
#include <string>

#include <context.hpp>
#include <config.hpp>
#include <common.hpp>
#include <worker.hpp>

namespace
{
  const char* vShader = R"shader(
    #version 330 core

    layout(location = 0) in vec3 pos;
    layout(location = 1) in vec2 uv;

    out vec2 fragUV;

    void main()
    {
      fragUV      = uv;
      gl_Position = vec4(pos, 1.0f);
    }
  )shader";

  const char* fShader = R"shader(
    #version 330 core

    layout(location = 0) out vec4 pixColor;

    in vec2 fragUV;

    uniform sampler2D mainTex;

    void main()
    {
      pixColor = texture(mainTex, fragUV);
    }
  )shader";

  const float vPos[6*3] = {
    -1.0f, -1.0f, 0.0f,
     1.0f, -1.0f, 0.0f,
     1.0f,  1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f,
     1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
  };

  const float vTex[6*2] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
  };

  const char* ErrorTypeToString(GLenum type)
  {
    switch(type)
    {
      case GL_DEBUG_TYPE_ERROR:
        return "ERROR";
      case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        return "DPRCD";
      case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        return "UNDEF";
      case GL_DEBUG_TYPE_PORTABILITY:
        return "PRTBL";
      case GL_DEBUG_TYPE_PERFORMANCE:
        return "PRFMN";
      case GL_DEBUG_TYPE_OTHER:
        return "OTHER";
      default:
        // Must not happen!
        assert(0);
        return "?????";
    }
  }
  
  const char* ErrorSeverity(GLenum severity)
  {
    switch(severity)
    {
      case GL_DEBUG_SEVERITY_LOW:
        return "~LOW";
      case GL_DEBUG_SEVERITY_MEDIUM:
        return "-MED";
      case GL_DEBUG_SEVERITY_HIGH:
        return "!HIG";
      case GL_DEBUG_SEVERITY_NOTIFICATION_KHR:
        return "!KHR";
      default:
        // Must not happen!
        assert(0);
        return "????";
    }
  }

  void APIENTRY OnGLError(GLenum /*source*/, GLenum type, GLuint /*id*/, GLenum severity, GLsizei /*length*/, const GLchar* message, const void* /*userPtr*/)
  {
    bb::Debug("OpenGL: [%s] [%s]\n\t%s", ErrorSeverity(severity), ErrorTypeToString(type), message);
  }

}

namespace bb
{

  context_t::context_t()
  :wnd(nullptr), width(800), height(600), canvas(),insideWnd(false),relativeCursor(false)
  {
    if (glfwInit() == GLFW_FALSE)
    {
      throw std::runtime_error("glfw initialization failed");
    }

    glfwWindowHint(GLFW_RESIZABLE, 0);

    config_t config;
    try
    {
      config.Load("default.config");
    }
    catch(const std::runtime_error&)
    {
      // ignore file not found error
      config["window.width"]  = ref_t::Number(800.0);
      config["window.height"] = ref_t::Number(600.0);
      config["window.title"] = ref_t::String("BadBaby");
      config["window.fullscreen"] = ref_t::Number(0.0);
      config["opengl.debug"] = ref_t::Number(0.0);
      config.Save("default.config");
    }

    if (config.Value("opengl.debug", 0.0) != 0.0)
    {
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
      glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    }
    else
    {
      glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
      glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
      glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_FALSE);
    }

    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    this->width  = config.Value("window.width",  800);
    this->height = config.Value("window.height", 600);
    std::string winTitle = config.Value("window.title", "BadBaby");
    bool winFullscreen = (config.Value("window.fullscreen", 0.0) != 0.0);

    if (winFullscreen)
    {
      auto monitor = glfwGetPrimaryMonitor();
      const GLFWvidmode* mode = glfwGetVideoMode(monitor);
      glfwWindowHint(GLFW_RED_BITS, mode->redBits);
      glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
      glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
      glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
      glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
      this->wnd = glfwCreateWindow(this->width, this->height, winTitle.c_str(), monitor, nullptr);
    }
    else
    {
      glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
      this->wnd = glfwCreateWindow(this->width, this->height, winTitle.c_str(), nullptr, nullptr);
    }
    if (this->wnd == nullptr)
    {
      throw std::runtime_error("glfw create window failed");
    }

    glfwSetWindowUserPointer(this->wnd, this);

    glfwSetCursorEnterCallback(this->wnd, context_t::OnCursorEnter);
    glfwSetKeyCallback(this->wnd, context_t::OnKey);

    glfwMakeContextCurrent(this->wnd);
    glfwSetWindowUserPointer(this->wnd, this);
    glfwSwapInterval(1);

    gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));

    if (config.Value("opengl.debug", 0.0) != 0.0)
    {
      glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
      glDebugMessageCallback(OnGLError, 0);
      glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }

    Info("OpenGL:\n\tVendor: %s\n\tRenderer: %s\n\tVersion: %s\n\tGLSL Version: %s",
      glGetString(GL_VENDOR),
      glGetString(GL_RENDERER),
      glGetString(GL_VERSION),
      glGetString(GL_SHADING_LANGUAGE_VERSION)
    );

    this->canvas = std::move(framebuffer_t(this->width, this->height));
    this->shader = std::move(shader_t(vShader, fShader));

    vbo_t vPosBuffer = vbo_t::CreateArrayBuffer(vPos, sizeof(vPos), false);
    vbo_t vTexBuffer = vbo_t::CreateArrayBuffer(vTex, sizeof(vTex), false);

    this->vao = vao_t::CreateVertexAttribObject();
    this->vao.BindVBO(vPosBuffer, 0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    this->vao.BindVBO(vTexBuffer, 1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }

  context_t::~context_t()
  {
    this->shader = std::move(shader_t());
    this->canvas = std::move(framebuffer_t());
    this->vao    = std::move(vao_t());

    if (this->wnd != nullptr)
    {
      glfwDestroyWindow(this->wnd);
    }
    glfwTerminate();
  }

  context_t& context_t::Instance()
  {
    static context_t self;
    return self;
  }

  bool context_t::Update()
  {
    framebuffer_t::RenderToScreen();
    shader_t::Bind(this->shader);
    vao_t::Bind(this->vao);
    texture_t::Bind(this->canvas.Texture());

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glFinish();
    glfwSwapBuffers(this->wnd);
    glfwPollEvents();

    return (glfwWindowShouldClose(this->wnd) == 0);
  }

  void context_t::Title(const std::string& newTitle)
  {
    glfwSetWindowTitle(this->wnd, newTitle.c_str());
  }

  void context_t::RelativeCursor(bool enable)
  {
    this->relativeCursor = enable;
    if (this->relativeCursor)
    {
      glfwSetInputMode(this->wnd, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    else
    {
      glfwSetInputMode(this->wnd, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
  }

  void context_t::OnCursorEnter(GLFWwindow* wnd, int entered)
  {
    Debug("CursorEnter: %d", entered);
    context_t* self = reinterpret_cast<context_t*>(glfwGetWindowUserPointer(wnd));
    self->insideWnd = (entered != 0);
    if (self->relativeCursor)
    {
      glfwSetInputMode(self->wnd, GLFW_CURSOR, (self->insideWnd)?GLFW_CURSOR_DISABLED:GLFW_CURSOR_NORMAL);
    }
  }

  void context_t::OnKey(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/)
  {
    context_t* self = reinterpret_cast<context_t*>(glfwGetWindowUserPointer(window));

    for (auto actorPair: self->actorCallbackList)
    {
      if ((actorPair.second & cmfKeyboard) != 0)
      {
        workerPool_t::Instance().PostMessage(actorPair.first, bb::MakeMsg(-1, msgID_t::KEYBOARD, keyEvent_t{ key, action }));
      }
    }
  }

  void context_t::RegisterActorCallback(int actorID, contextMsgFlag_t flags)
  {
    this->actorCallbackList.emplace_back(std::make_pair(actorID, flags));
  }

  void context_t::UnregisterActorCallbacks(int actorID)
  {
    assert(actorID >= 0);

    this->actorCallbackList.remove_if(
      [actorID](const pairOfFlags& pof) -> bool
      {
        return (pof.first == actorID);
      }
    );
  }

} // namespace bb
