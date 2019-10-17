#include <stdexcept>
#include <string>

#include <context.hpp>

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

    vbo_t vPosBuffer = vbo_t::CreateArrayBuffer(vPos, sizeof(vPos));
    vbo_t vTexBuffer = vbo_t::CreateArrayBuffer(vTex, sizeof(vTex));

    this->vao = vao_t::CreateVertexAttribObject();
    this->vao.BindVBO(vPosBuffer, 0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    this->vao.BindVBO(vTexBuffer, 1, 2, GL_FLOAT, GL_FALSE, 0, 0);
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
    framebuffer_t::Bind(framebuffer_t());
    shader_t::Bind(this->shader);
    vao_t::Bind(this->vao);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glfwSwapBuffers(this->wnd);
    glfwWaitEvents();

    return (glfwWindowShouldClose(this->wnd) == 0);
  }

} // namespace bb
