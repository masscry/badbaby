#include <stdexcept>
#include <string>

#include <context.hpp>

namespace
{
  const char* vShader = R"shader(
    #version 330 core

    layout(location = 0) in vec3 pos;
    layout(location = 3) in vec2 uv;

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
}

namespace bb
{

  context_t::context_t()
  :wnd(nullptr), width(800), height(600), canvas()
  {
    if (glfwInit() == GLFW_FALSE)
    {
      throw std::runtime_error("glfw initialization failed");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_RESIZABLE, 0);

    this->wnd = glfwCreateWindow(this->width, this->height, "BadBaby", nullptr, nullptr);
    if (this->wnd == nullptr)
    {
      throw std::runtime_error("glfw create window failed");
    }

    glfwMakeContextCurrent(this->wnd);
    glfwSetWindowUserPointer(this->wnd, this);
    glfwSwapInterval(1);

    gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));

    this->canvas = std::move(framebuffer_t(this->width, this->height));
    this->shader = std::move(shader_t(vShader, fShader));
  }

  context_t::~context_t()
  {
    this->shader = std::move(shader_t());
    this->canvas = std::move(framebuffer_t());

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
    framebuffer_t::Bind(framebuffer_t());
    shader_t::Bind(this->shader);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwSwapBuffers(this->wnd);
    glfwWaitEvents();

    return (glfwWindowShouldClose(this->wnd) == 0);
  }

} // namespace bb
