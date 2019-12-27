#include <mainMenu.hpp>

#include <glm/glm.hpp>

#include <config.hpp>

#include <actor.hpp>
#include <msg.hpp>
#include <worker.hpp>
#include <sub3000.hpp>

namespace sub3000
{

  enum mainMenuSceneMsg_t
  {
    MMS_SELECT_LINE = 0
  };

  void mainMenuModel_t::ProcessMessage(bb::msg_t msg)
  {
    switch (msg.type)
    {
      case -bb::cmfKeyboard:
      {
        auto keyEvent = bb::GetMsgData<bb::keyEvent_t>(msg);
        if (keyEvent.press != GLFW_RELEASE)
        {
          switch(keyEvent.key)
          {
            case GLFW_KEY_UP:
              if (this->selectedLine > 0)
              {
                --this->selectedLine;
                this->view.Put(bb::MakeMsg(0, MMS_SELECT_LINE, this->selectedLine));
              }
              break;
            case GLFW_KEY_DOWN:
              if (this->selectedLine < this->textList.size()-1)
              {
                ++this->selectedLine;
                this->view.Put(bb::MakeMsg(0, MMS_SELECT_LINE, this->selectedLine));
              }
              break;
            case GLFW_KEY_ENTER:
              PostToMain(bb::MakeMsg(0, sub3000::mainMessage_t::action, this->textList[this->selectedLine].action));
              break;
          }
        }
      }
      break;
      default:
        assert(0);
    }
  }

  mainMenuModel_t::mainMenuModel_t(textList_t& textList, bb::mailbox_t& view)
  : textList(textList),
    view(view),
    selectedLine(0)    
  {
    this->SetName("MainMenu");
    this->view.Put(bb::MakeMsg(0, MMS_SELECT_LINE, selectedLine));
  }

  mainMenuScene_t::menuLines_t mainMenuScene_t::LoadMenuLines(const bb::config_t& config, const std::string& prefix)
  {
    mainMenuScene_t::menuLines_t result;

    int totalLines = static_cast<int>(config["menu.lines"].Number());

    result.reserve(totalLines);
    for (int i = 0; i < totalLines; ++i)
    {
      std::string menu_text = prefix + std::string("[") + std::to_string(i) + "]";
      result.push_back(config[menu_text].String());
    }
    return result;
  }

  void mainMenuScene_t::OnPrepare()
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

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    int line = 0;

    for(size_t i = 0, e = this->textLines.size(); i < e; ++i)
    {
      this->textList.emplace_back(
          bb::textDynamic_t(font, this->textSize),
          bb::node_t(),
          GetActionFromText(this->msgLines[i])
      );

      this->textList.back().text.Update(this->textLines[i]);
      this->textList.back().node.Reset();
      this->textList.back().node.Translate(bb::vec3_t(0.0f, this->textSize.y/2.0f, 0.0f));
      this->textList.back().node.Scale(bb::vec3_t(1.0f, -1.0f, 1.0f));
      this->textList.back().node.Translate(bb::vec3_t(0.0f, -line*this->textSize.y - this->textSize.y/2.0f, 0.0f));
      ++line;
    }

    auto& pool = bb::workerPool_t::Instance();
    this->menuModelID = pool.Register(std::unique_ptr<bb::actor_t>(new sub3000::mainMenuModel_t(this->textList, this->mailbox)));
    this->pContext->RegisterActorCallback(menuModelID, bb::cmfKeyboard);
  }

  void mainMenuScene_t::OnUpdate(double)
  {
    bb::msg_t msg;
    while(this->mailbox.Poll(&msg))
    {
      switch(msg.type)
      {
        case MMS_SELECT_LINE:
          this->selectedMenuLine = bb::GetMsgData<uint32_t>(msg);
          break;
        default:
          // unknown action!
          assert(0);
      }
    }
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

    uint32_t line = 0;
    for(auto&& item: this->textList)
    {
      bb::vec3_t itemColor;

      if (this->selectedMenuLine == line)
      {
        itemColor.r = 1.0f;
        itemColor.g = 0.4f;
        itemColor.b = 0.4f;
      }
      else
      {
        itemColor.r = 0.4f;
        itemColor.g = 0.4f;
        itemColor.b = 0.4f;
      }
      
      this->shader.SetVector3f(this->glyphColorBindPoint, 1, &itemColor.x);
      this->shader.SetMatrix(this->modelBindPoint, &item.node.Model()[0][0]);
      item.text.Render();
      ++line;
    }
  }

  void mainMenuScene_t::OnCleanup()
  {
    auto& pool = bb::workerPool_t::Instance();
    pool.Unregister(this->menuModelID);
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
    this->textSize.x = splashConfig["text.width"].Number();
    this->textSize.y = splashConfig["text.height"].Number();

    this->textLines = this->LoadMenuLines(splashConfig, "menu.text");
    this->msgLines  = this->LoadMenuLines(splashConfig, "menu.msg");
  }

  mainMenuScene_t::~mainMenuScene_t()
  {

  }

} // namespace sub3000