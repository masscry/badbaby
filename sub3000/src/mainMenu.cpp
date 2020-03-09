#include <mainMenu.hpp>

#include <glm/glm.hpp>

#include <config.hpp>

#include <actor.hpp>
#include <msg.hpp>
#include <worker.hpp>
#include <sub3000.hpp>

#include <mapGen.hpp>

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

  class selectLine_t: public bb::msg::basic_t
  {
    uint32_t line;
  public:

    uint32_t Line() const
    {
      return this->line;
    }

    selectLine_t(uint32_t line)
    : line(line)
    {
      ;
    }

    selectLine_t(const selectLine_t&) = default;
    selectLine_t& operator= (const selectLine_t&) = default;
    selectLine_t(selectLine_t&&) = default;
    selectLine_t& operator= (selectLine_t&&) = default;
    ~selectLine_t() override = default;
  };

  bb::msg::result_t mainMenuModel_t::OnProcessMessage(const bb::actor_t& actor, const bb::msg::basic_t& msg)
  {
    if (auto keyEvent = bb::msg::As<bb::msg::keyEvent_t>(msg))
    {
      if (keyEvent->Press() != GLFW_RELEASE)
      {
        switch(keyEvent->Key())
        {
          case GLFW_KEY_UP:
            if (this->selectedLine > 0)
            {
              --this->selectedLine;
              this->view.Put(
                bb::Issue<selectLine_t>(this->selectedLine)
              );
            }
            break;
          case GLFW_KEY_DOWN:
            if (this->selectedLine < this->textList.size()-1)
            {
              ++this->selectedLine;
              this->view.Put(
                bb::Issue<selectLine_t>(this->selectedLine)
              );
            }
            break;
          case GLFW_KEY_ENTER:
            PostToMain(
              bb::Issue<sub3000::action_t>(
                actor.ID(),
                this->textList[this->selectedLine].action
              )
            );
            break;
        }
      }
      return bb::msg::result_t::complete;
    }

    if (auto done = bb::msg::As<sub3000::done_t>(msg))
    {
      bb::Debug("Map Generation Done: %d %d",
        done->HeightMap().width,
        done->HeightMap().height
      );
      return bb::msg::result_t::complete;
    }

    bb::Error("Unknown message %s", typeid(msg).name());
    assert(0);
    return bb::msg::result_t::error;
  }

  mainMenuModel_t::mainMenuModel_t(textList_t& textList, bb::mailbox_t& view)
  : textList(textList),
    view(view),
    selectedLine(0)
  {
    this->view.Put(
      bb::Issue<selectLine_t>(selectedLine)
    );
  }

  mainMenuScene_t::menuLines_t mainMenuScene_t::LoadMenuLines(const bb::config_t& config, const std::string& prefix)
  {
    mainMenuScene_t::menuLines_t result;

    size_t totalLines = static_cast<size_t>(config["menu.lines"].Number());

    result.reserve(totalLines);
    for (size_t i = 0; i < totalLines; ++i)
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
      0.0f, static_cast<float>(this->pContext->Width()),
      0.0f, static_cast<float>(this->pContext->Height())
    );
    this->camera.Projection() = glm::scale(this->camera.Projection(), bb::vec3_t(1.0f, -1.0f, 1.0f));
    this->camera.Projection() = glm::translate(this->camera.Projection(), bb::vec3_t(0.0f, -this->pContext->Height(), 0.0f));

    this->camBindBlock        = this->shader.UniformBlockIndex("camera");
    this->modelBindPoint      = this->shader.UniformLocation("model");
    this->glyphColorBindPoint = this->shader.UniformLocation("glyphColor");

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    float line = 0.0f;

    float maxCharacters = static_cast<float>(MaxStringLengthInArray(this->textLines));

    auto textOffset = bb::vec3_t(
      static_cast<float>(this->pContext->Width())/2.0f - maxCharacters*this->textSize.x/4.0f,
      static_cast<float>(-this->pContext->Height())/2.0f + static_cast<float>(this->textLines.size())*this->textSize.y/2.0f,
      0.0f
    );

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
      textListBack.node.Translate(
        bb::vec3_t(
          0.0f,
          -line*this->textSize.y - this->textSize.y/2.0f,
          0.0f
        )
      );
      textListBack.node.Translate(textOffset);

      line += 1.0f;
    }

    this->gameInfoText = bb::textStatic_t(this->font, this->gameInfo, this->textSize*1.6f, 0);
    this->gameInfoNode.Reset();
    this->gameInfoNode.Translate(bb::vec3_t(0.0f, this->textSize.y*1.6f/2.0f, 0.0f));
    this->gameInfoNode.Scale(bb::vec3_t(1.0f, -1.0f, 1.0f));
    this->gameInfoNode.Translate(bb::vec3_t(100.0f, -100.0f, 0.0f));

    auto& pool = bb::workerPool_t::Instance();
    this->menuModelID = pool.Register(std::unique_ptr<bb::role_t>(new sub3000::mainMenuModel_t(this->textList, *this->mailbox)));
    this->pContext->RegisterActorCallback(menuModelID, bb::cmfKeyboard);
  }

  void mainMenuScene_t::OnUpdate(double)
  {
    bb::msg_t msg;
    while(this->mailbox->Poll(&msg))
    {
      if (auto selectedLine = bb::As<sub3000::selectLine_t>(msg))
      {
        this->selectedMenuLine = selectedLine->Line();
        continue;
      }

      if (auto* defMsg = msg.get())
      {
        bb::Error("Unknown message: %s", typeid(*defMsg).name());
      }
      else
      {
        bb::Error("Unknown message: %s", "Invalid Message!");
      }

      assert(0);
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
    pContext(nullptr),
    mailbox(bb::postOffice_t::Instance().New("mainMenuScene_t"))
  {
    bb::config_t menuConfig;
    menuConfig.Load("./mainMenu.config");

    this->shader_vp  = menuConfig["shader.vp"].String();
    this->shader_fp  = menuConfig["shader.fp"].String();
    this->fontConfig = menuConfig["menu.font"].String();
    this->textSize.x = static_cast<float>(menuConfig["text.width"].Number());
    this->textSize.y = static_cast<float>(menuConfig["text.height"].Number());

    this->textLines = this->LoadMenuLines(menuConfig, "menu.text");
    this->msgLines  = this->LoadMenuLines(menuConfig, "menu.msg");
    this->gameInfo  = menuConfig["game.info"].String();
  }

  mainMenuScene_t::~mainMenuScene_t()
  {

  }

} // namespace sub3000
