#include <mainMenu.hpp>

#include <glm/glm.hpp>

#include <config.hpp>

#include <actor.hpp>
#include <msg.hpp>
#include <worker.hpp>
#include <sub3000.hpp>

namespace
{

  template<typename array_t>
  size_t MaxStringLengthInArray(const array_t& array)
  {
    size_t result = 0;
    for(const auto& item: array)
    {
      result = std::max(result, item.size());
    }
    return result;
  }

}

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

    size_t maxCharacters = MaxStringLengthInArray(this->textLines);

    for(size_t i = 0, e = this->textLines.size(); i < e; ++i)
    {
      this->textList.emplace_back(
          bb::textDynamic_t(this->font, this->textSize),
          bb::node_t(),
          GetActionFromText(this->msgLines[i])
      );

      auto& textListBack = this->textList.back();

      textListBack.text.Update(this->textLines[i]);
      textListBack.node.Reset();
      textListBack.node.Translate(bb::vec3_t(0.0f, this->textSize.y/2.0f, 0.0f));
      textListBack.node.Scale(bb::vec3_t(1.0f, -1.0f, 1.0f));
      textListBack.node.Translate(bb::vec3_t(0.0f, -line*this->textSize.y - this->textSize.y/2.0f, 0.0f));
      textListBack.node.Translate(
        bb::vec3_t(
          this->pContext->Width()/2.0f - maxCharacters*this->textSize.x/4.0,
          -this->pContext->Height()/2.0f + e*this->textSize.y/2.0f,
          0.0f
        )
      );

      ++line;
    }

    this->gameInfoText = bb::textStatic_t(this->font, this->gameInfo, this->textSize*1.6f, 0);
    this->gameInfoNode.Reset();
    this->gameInfoNode.Translate(bb::vec3_t(0.0f, this->textSize.y*1.6f/2.0f, 0.0f));
    this->gameInfoNode.Scale(bb::vec3_t(1.0f, -1.0f, 1.0f));
    this->gameInfoNode.Translate(bb::vec3_t(100.0f, -100.0f, 0.0f));

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

  static const bb::vec3_t infoNodeColor = {
    0.7f, 0.7f, 0.7f
  };

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
      
      this->shader.SetVector3f(this->glyphColorBindPoint, 1, &itemColor.r);
      this->shader.SetMatrix(this->modelBindPoint, &item.node.Model()[0][0]);
      item.text.Render();
      ++line;
    }

    this->shader.SetVector3f(this->glyphColorBindPoint, 1, &infoNodeColor.r);
    this->shader.SetMatrix(this->modelBindPoint, &this->gameInfoNode.Model()[0][0]);
    this->gameInfoText.Render();

  }

  void mainMenuScene_t::OnCleanup()
  {
    auto& pool = bb::workerPool_t::Instance();
    pool.Unregister(this->menuModelID);
    this->textList.clear();
  }

  mainMenuScene_t::mainMenuScene_t()
  : scene_t(sceneID_t::mainMenu, "Main Menu"),
    pContext(nullptr)
  {
    bb::config_t menuConfig;
    menuConfig.Load("./mainMenu.config");

    this->shader_vp  = menuConfig["shader.vp"].String();
    this->shader_fp  = menuConfig["shader.fp"].String();
    this->fontConfig = menuConfig["menu.font"].String();
    this->textSize.x = menuConfig["text.width"].Number();
    this->textSize.y = menuConfig["text.height"].Number();

    this->textLines = this->LoadMenuLines(menuConfig, "menu.text");
    this->msgLines  = this->LoadMenuLines(menuConfig, "menu.msg");
    this->gameInfo  = menuConfig["game.info"].String();
  }

  mainMenuScene_t::~mainMenuScene_t()
  {

  }

} // namespace sub3000