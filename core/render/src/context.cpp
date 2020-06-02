#include <cassert>

#include <stdexcept>
#include <string>

#include <common.hpp>
#include <config.hpp>
#include <context.hpp>
#include <worker.hpp>

#ifdef _WIN32

// Enable high perfomance video card by default

extern "C"
{
  _declspec(dllexport) uint32_t NvOptimusEnablement = 0x00000001;
}

#endif

namespace
{
  const char *vShader = R"shader(
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

  const char *fShader = R"shader(
    #version 330 core

    layout(location = 0) out vec4 pixColor;

    in vec2 fragUV;

    uniform sampler2D mainTex;

    void main()
    {
      pixColor = texture(mainTex, fragUV);
    }
  )shader";

  const float vPos[6 * 3] = {
    -1.0f,
    -1.0f,
    0.0f,
    1.0f,
    -1.0f,
    0.0f,
    1.0f,
    1.0f,
    0.0f,
    -1.0f,
    -1.0f,
    0.0f,
    1.0f,
    1.0f,
    0.0f,
    -1.0f,
    1.0f,
    0.0f,
  };

  const float vTex[6 * 2] = {
    0.0f,
    0.0f,
    1.0f,
    0.0f,
    1.0f,
    1.0f,
    0.0f,
    0.0f,
    1.0f,
    1.0f,
    0.0f,
    1.0f,
  };

  const char *ErrorTypeToString(GLenum type)
  {
    switch (type)
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

  const char *ErrorSeverity(GLenum severity)
  {
    switch (severity)
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

  const char *ErrorSource(GLenum source)
  {
    switch (source)
    {
      case GL_DEBUG_SOURCE_API_KHR:
        return "SRC";
      case GL_DEBUG_SOURCE_WINDOW_SYSTEM_KHR:
        return "WND";
      case GL_DEBUG_SOURCE_SHADER_COMPILER_KHR:
        return "SHD";
      case GL_DEBUG_SOURCE_THIRD_PARTY_KHR:
        return "3RD";
      case GL_DEBUG_SOURCE_APPLICATION_KHR:
        return "APP";
      case GL_DEBUG_SOURCE_OTHER_KHR:
        return "OTH";
      default:
        // Must not happen!
        assert(0);
        return "???";
    }
  }

  void APIENTRY OnGLError(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei /*length*/, const GLchar *message, const void * /*userPtr*/)
  {
    bb::Debug(
      "=== OPENGL ===\n\tSTATE:\t[%s] [%s] [%s]\n\tMSG:\t%s\n\tID:\t0x%08x",
      ErrorSource(source),
      ErrorSeverity(severity),
      ErrorTypeToString(type),
      message,
      id);
    assert(severity != GL_DEBUG_SEVERITY_HIGH);
  }

} // namespace

namespace bb
{

  context_t::context_t()
      : wnd(nullptr), width(800), height(600), canvas(), insideWnd(false), relativeCursor(false), hasNewTitle(false)
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
    catch (const std::runtime_error &)
    {
      // ignore file not found error
      config["window.width"] = ref_t::Number(800.0);
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
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    this->width = static_cast<int>(config.Value("window.width", 800.0));
    this->height = static_cast<int>(config.Value("window.height", 600.0));
    std::string winTitle = config.Value("window.title", "BadBaby");
    bool winFullscreen = (config.Value("window.fullscreen", 0.0) != 0.0);

    if (winFullscreen)
    {
      auto monitor = glfwGetPrimaryMonitor();
      const GLFWvidmode *mode = glfwGetVideoMode(monitor);
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
      glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION_KHR, 0, nullptr, GL_FALSE);
      glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_OTHER, GL_DONT_CARE, 0, nullptr, GL_FALSE);
    }

    Info("OpenGL:\n\tVendor: %s\n\tRenderer: %s\n\tVersion: %s\n\tGLSL Version: %s",
      glGetString(GL_VENDOR),
      glGetString(GL_RENDERER),
      glGetString(GL_VERSION),
      glGetString(GL_SHADING_LANGUAGE_VERSION));

    this->canvas = framebuffer_t(this->width, this->height);
    this->shader = shader_t(vShader, fShader);

    vbo_t vPosBuffer = vbo_t::CreateArrayBuffer(vPos, sizeof(vPos), false);
    vbo_t vTexBuffer = vbo_t::CreateArrayBuffer(vTex, sizeof(vTex), false);

    this->vao = vao_t::CreateVertexAttribObject();
    this->vao.BindVBO(vPosBuffer, 0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    this->vao.BindVBO(vTexBuffer, 1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    context_t::isAlreadyExists = true;
  }

  context_t::~context_t()
  {
    this->shader = shader_t();
    this->canvas = framebuffer_t();
    this->vao = vao_t();

    if (this->wnd != nullptr)
    {
      glfwDestroyWindow(this->wnd);
    }
    glfwTerminate();
    context_t::isAlreadyExists = false;
  }

  context_t &context_t::Instance()
  {
    static context_t self;
    return self;
  }

  bool context_t::isAlreadyExists = false;

  bool context_t::IsAlreadyExists()
  {
    return context_t::isAlreadyExists;
  }

  bool context_t::Update()
  {
    std::unique_lock<std::mutex> lock(this->mutex);

    if (this->hasNewTitle)
    {
      glfwSetWindowTitle(this->wnd, this->title.c_str());
      this->hasNewTitle = false;
    }
   
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

  void context_t::Title(const std::string &newTitle)
  {
    std::unique_lock<std::mutex> lock(this->mutex);
    this->title = newTitle;
    this->hasNewTitle = true;
  }

  void context_t::RelativeCursor(bool enable)
  {
    std::unique_lock<std::mutex> lock(this->mutex);
    this->relativeCursor = enable;

    glfwSetInputMode(this->wnd, GLFW_CURSOR,
      ((this->relativeCursor) && (this->insideWnd)) ?(GLFW_CURSOR_DISABLED):(GLFW_CURSOR_NORMAL)
    );
  }

  void context_t::OnCursorEnter(GLFWwindow *wnd, int entered)
  { // this function called is called when glfwPollEvents happens
    // at that moment context mutex is already aquired by update thread
    // so, no lock here

    context_t *self = reinterpret_cast<context_t *>(glfwGetWindowUserPointer(wnd));
    self->insideWnd = (entered != GLFW_FALSE);
    
    glfwSetInputMode(self->wnd, GLFW_CURSOR,
      ((self->relativeCursor) && (self->insideWnd)) ?(GLFW_CURSOR_DISABLED):(GLFW_CURSOR_NORMAL)
    );
  }

  void context_t::OnKey(GLFWwindow *window, int key, int /*scancode*/, int action, int /*mods*/)
  { // this function called is called when glfwPollEvents happens
    // at that moment context mutex is already aquired by update thread
    // so, no lock here

    context_t *self = reinterpret_cast<context_t *>(glfwGetWindowUserPointer(window));
    for (auto actorPair : self->actorCallbackList)
    {
      if ((actorPair.second & msgFlag_t::keyboard) != 0)
      {
        workerPool_t::Instance().PostMessage(actorPair.first,
          bb::msg_t(
            new bb::msg::keyEvent_t(key, action)
          )
        );
      }
    }
  }

  void context_t::RegisterActorCallback(actorPID_t actorID, context_t::msgFlag_t flags)
  {
    std::unique_lock<std::mutex> lock(this->mutex);
    this->actorCallbackList.emplace_back(std::make_pair(actorID, flags));
  }

  void context_t::UnregisterActorCallbacks(actorPID_t actorID)
  {
    std::unique_lock<std::mutex> lock(this->mutex);
    this->actorCallbackList.remove_if(
      [actorID](const pairOfFlags &pof) -> bool {
        return (pof.first == actorID);
      });
  }

  size_t TypeSize(GLenum type)
  {
    switch (type)
    {
      case GL_BYTE:
      case GL_UNSIGNED_BYTE:
        return 1;
      case GL_SHORT:
      case GL_UNSIGNED_SHORT:
      case GL_HALF_FLOAT:
        return 2;
      case GL_INT:
      case GL_UNSIGNED_INT:
      case GL_FIXED:
      case GL_FLOAT:
        return 4;
      default:
        // programmer's error
        bb::Error("Unknown type: (%d)", type);
        assert(0);
        return 0;
    }
  }

} // namespace bb
