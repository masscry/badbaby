#include <authors.hpp>

#include <glm/glm.hpp>

#include <config.hpp>

#include <actor.hpp>
#include <msg.hpp>
#include <worker.hpp>
#include <sub3000.hpp>

#include <fstream>
#include <sstream>

namespace
{

  std::string LoadFileAsString(const std::string& inputFilename)
  {
    std::ifstream input(inputFilename, std::ios::in);
    std::stringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
  }

}

namespace sub3000
{

  bb::msgResult_t authorsModel_t::OnProcessMessage(const bb::actor_t&, bb::msg_t msg)
  {
    switch (msg.type)
    {
      case bb::msgID_t::KEYBOARD:
      {
        auto keyEvent = bb::GetMsgData<bb::keyEvent_t>(msg);
        if (keyEvent.press != GLFW_RELEASE)
        {
          PostChangeScene(sub3000::sceneID_t::mainMenu);
        }
      }
      break;
      default:
        assert(0);
    }
    return bb::msgResult_t::complete;
  }

  authorsModel_t::authorsModel_t()
  {
    ;
  }


  void authorsScene_t::OnPrepare()
  {
    this->pContext = &bb::context_t::Instance();
    this->shader = bb::shader_t::LoadProgramFromFiles(
      this->shader_vp.c_str(), 
      this->shader_fp.c_str()
    );

    this->font = bb::font_t(this->fontConfig);
    this->camera = bb::camera_t::Orthogonal(
      0.0f, this->pContext->Width(),
      0.0f, this->pContext->Height()
    );
    this->camera.Projection() = glm::scale(this->camera.Projection(), bb::vec3_t(1.0f, -1.0f, 1.0f));
    this->camera.Projection() = glm::translate(this->camera.Projection(), bb::vec3_t(0.0f, -this->pContext->Height(), 0.0f));

    this->camBindBlock        = this->shader.UniformBlockIndex("camera");
    this->modelBindPoint      = this->shader.UniformLocation("model");
    this->glyphColorBindPoint = this->shader.UniformLocation("glyphColor");

    glClearColor(0.2f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    float textBias = (this->pContext->Width() - this->maxLineWidth*this->textSize.x)/2.0f;

    std::string authorsText = LoadFileAsString(this->authorsFilename);

    this->authorsText = bb::textStatic_t(this->font, authorsText, this->textSize, this->maxLineWidth);
    this->authorsNode.Reset();
    this->authorsNode.Translate(bb::vec3_t(textBias, this->textSize.y, 0.0f));
    this->authorsNode.Scale(bb::vec3_t(1.0f, -1.0f, 1.0f));


    auto& pool = bb::workerPool_t::Instance();
    this->model = pool.Register(std::unique_ptr<bb::role_t>(new sub3000::authorsModel_t()));
    this->pContext->RegisterActorCallback(this->model, bb::cmfKeyboard);
  }

  void authorsScene_t::OnUpdate(double)
  {
  }

  static const bb::vec3_t authorsColor = {
    0.5f, 0.5f, 0.5f
  };

  void authorsScene_t::OnRender()
  {
    bb::framebuffer_t::Bind(this->pContext->Canvas());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    bb::shader_t::Bind(this->shader);
    this->camera.Update();
    this->shader.SetBlock(
      this->camBindBlock,
      this->camera.UniformBlock()
    );

    this->shader.SetVector3f(this->glyphColorBindPoint, 1, &authorsColor.r);
    this->shader.SetMatrix(this->modelBindPoint, &this->authorsNode.Model()[0][0]);
    this->authorsText.Render();
  }

  void authorsScene_t::OnCleanup()
  {
    auto& pool = bb::workerPool_t::Instance();
    pool.Unregister(this->model);
  }

  authorsScene_t::authorsScene_t()
  : scene_t(sceneID_t::mainMenu, "Authors"),
    pContext(nullptr)
  {
    bb::config_t authorsConfig;
    authorsConfig.Load("./authors.config");

    this->shader_vp  = authorsConfig["shader.vp"].String();
    this->shader_fp  = authorsConfig["shader.fp"].String();
    this->fontConfig = authorsConfig["menu.font"].String();
    this->textSize.x = authorsConfig["text.width"].Number();
    this->textSize.y = authorsConfig["text.height"].Number();

    this->authorsFilename = authorsConfig["authors.info"].String();
    this->maxLineWidth = static_cast<size_t>(authorsConfig["text.maxline"].Number());
  }

  authorsScene_t::~authorsScene_t()
  {

  }



}