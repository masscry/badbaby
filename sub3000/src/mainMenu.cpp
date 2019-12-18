#include <mainMenu.hpp>

#include <glm/glm.hpp>

#include <config.hpp>

namespace sub3000
{

  void mainMenuScene_t::OnPrepare()
  {
    this->pContext = &bb::context_t::Instance();
    this->shader = bb::shader_t::LoadProgramFromFiles(
      this->shader_vp.c_str(), 
      this->shader_fp.c_str()
    );

    this->font = bb::font_t(this->fontConfig);
    this->text = bb::textDynamic_t(font, bb::vec2_t(32.0, 128.0));
    text.Update("Главное меню");
    this->camera = bb::camera_t::Orthogonal(
      0.0f, this->pContext->Width(),
      0.0f, this->pContext->Height()
    );

    this->camBindBlock = this->shader.UniformBlockIndex("camera");

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

  }

  void mainMenuScene_t::OnUpdate(double)
  {
    ;
  }

  void mainMenuScene_t::OnRender()
  {
    bb::framebuffer_t::Bind(this->pContext->Canvas());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    bb::shader_t::Bind(this->shader);
    this->camera.Update();
    this->shader.SetBlock(
      this->camBindBlock,
      this->camera.UniformBlock()
    );

    this->text.Render();
  }

  void mainMenuScene_t::OnCleanup()
  {

  }

  mainMenuScene_t::mainMenuScene_t()
  : scene_t(sceneID_t::mainMenu, "Main Menu"),
    pContext(nullptr)
  {
    bb::config_t splashConfig;
    splashConfig.Load("./mainMenu.config");

    this->shader_vp  = splashConfig["shader.vp"].String();
    this->shader_fp  = splashConfig["shader.fp"].String();
    this->fontConfig = splashConfig["menu.font"].String();
  }

  mainMenuScene_t::~mainMenuScene_t()
  {

  }

} // namespace sub3000