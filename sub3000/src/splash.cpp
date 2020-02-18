#include <splash.hpp>

#include <glm/glm.hpp>

#include <config.hpp>

namespace
{
  glm::vec2 vPos[4] = {
    { -0.5f, -0.5f },
    { +0.5f, -0.5f },
    { +0.5f, +0.5f },
    { -0.5f, +0.5f}
  };

  glm::vec2 vUV[4] = {
    { 0.0f, 1.0f },
    { 1.0f, 1.0f },
    { 1.0f, 0.0f },
    { 0.0f, 0.0f }
  };

  uint16_t vInd[6] = {
    0, 1, 2, 0, 2, 3
  };

} // namespace


namespace sub3000
{

  void splashScene_t::OnPrepare()
  {
    this->pContext = &bb::context_t::Instance();
    this->shader = bb::shader_t::LoadProgramFromFiles(
      this->shader_vp.c_str(),
      this->shader_fp.c_str()
    );
    this->timePassedUniform = this->shader.UniformLocation("time");

    this->texture = bb::texture_t::LoadTGA(this->logo);

    auto vboPos  = bb::vbo_t::CreateArrayBuffer(vPos, sizeof(vPos), false);
    auto vboUV   = bb::vbo_t::CreateArrayBuffer(vUV,  sizeof(vUV),  false);
    auto vboInd  = bb::vbo_t::CreateElementArrayBuffer(vInd, sizeof(vInd), false);
    this->object = bb::vao_t::CreateVertexAttribObject();

    this->object.BindVBO(vboPos, 0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    this->object.BindVBO(vboUV,  1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    this->object.BindIndecies(vboInd);

    this->currentTimePassed = 0.0;
  }

  void splashScene_t::OnUpdate(double delta)
  {
    this->currentTimePassed += delta;
    if (this->currentTimePassed > this->duration)
    {
      PostChangeScene(sceneID_t::mainMenu);
    }
  }

  void splashScene_t::OnRender()
  {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glDisable(GL_DEPTH_TEST);

    bb::shader_t::Bind(this->shader);
    this->shader.SetFloat(
      this->timePassedUniform,
      static_cast<float>(this->currentTimePassed)
    );

    bb::framebuffer_t::Bind(this->pContext->Canvas());
    bb::vao_t::Bind(this->object);
    bb::texture_t::Bind(this->texture);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
  }

  void splashScene_t::OnCleanup()
  {
    ;
  }

  splashScene_t::splashScene_t()
  : scene_t(sceneID_t::splash, "Splash Screen"),
    duration(0.0),
    timePassedUniform(0),
    currentTimePassed(0.0),
    pContext(nullptr)
  {
    bb::config_t splashConfig;
    splashConfig.Load("./logo.config");

    this->duration  = splashConfig["duration"].Number();
    this->shader_vp = splashConfig["shader.vp"].String();
    this->shader_fp = splashConfig["shader.fp"].String();
    this->logo      = splashConfig["logo.texture"].String();
  }

  splashScene_t::~splashScene_t()
  {

  }

} // namespace sub3000
