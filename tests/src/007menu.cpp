#include <random>
#include <memory>
#include <cmath>

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <common.hpp>
#include <context.hpp>

#include <actor.hpp>
#include <worker.hpp>
#include <role.hpp>
#include <msg.hpp>

#include <font.hpp>
#include <text.hpp>

enum menuLine_t
{
  ML_FIRST = 0,
  ML_NEW_GAME = ML_FIRST,
  ML_LOAD_GAME,
  ML_ABOUT,
  ML_EXIT,
  ML_LAST = ML_EXIT,
  ML_TOTAL
};

const char* menuTextLines[ML_TOTAL] = {
  "Новая Игра",
  "Загрузить Игру",
  "Авторы",
  "Выход"
};

bb::mailbox_t::shared_t renderTasks = bb::postOffice_t::Instance().New("RenderTasks");

namespace menu
{

  class action_t final: public bb::msg::basic_t
  {
  public:

    enum id_t
    {
      prev,
      next,
      select
    };

  private:

    id_t id;

  public:

    id_t ID() const
    {
      return this->id;
    }

    action_t(id_t id)
    : id(id)
    {
      ;
    }

    action_t(const action_t&) = default;
    action_t& operator=(const action_t&) = default;
    action_t(action_t&&) = default;
    action_t& operator=(action_t&&) = default;
    ~action_t() override = default;
  };

  class animation_t final: public bb::msg::basic_t
  {
    int newLine;
  public:

    int NewLine() const
    {
      return this->newLine;
    }

    animation_t(int newLine)
    : newLine(newLine)
    {
      ;
    }
    animation_t(const animation_t&) = default;
    animation_t& operator= (const animation_t&) = default;
    animation_t(animation_t&&) = default;
    animation_t& operator= (animation_t&&) = default;
    ~animation_t() override = default;
  };

  class exit_t: public bb::msg::basic_t
  {
  public:
    exit_t() { ; }
    ~exit_t() override = default;
  };

  class done_t: public bb::msg::basic_t
  {
    int selected;
  public:

    int Selected() const
    {
      return this->selected;
    }

    done_t(int selected)
    : selected(selected)
    {
      ;
    }

    done_t(const done_t&) = default;
    done_t& operator=(const done_t&) = default;
    done_t(done_t&&) = default;
    done_t& operator=(done_t&&) = default;
    ~done_t() override = default;

  };

  class model_t: public bb::role_t
  {
    int selected;

    void PrevLine()
    {
      int nextMenuLine = this->selected;
      nextMenuLine = (((nextMenuLine-1) < ML_FIRST)?(ML_LAST):(nextMenuLine-1));
      renderTasks->Put(
        bb::Issue<menu::animation_t>(nextMenuLine)
      );
    }

    void NextLine()
    {
      int nextMenuLine = this->selected;
      nextMenuLine = (((nextMenuLine+1) > ML_LAST)?(ML_FIRST):(nextMenuLine+1));
      renderTasks->Put(
        bb::Issue<menu::animation_t>(nextMenuLine)
      );
    }

    void SelectLine()
    {
      if (this->selected == ML_EXIT)
      {
        renderTasks->Put(
          bb::Issue<menu::exit_t>()
        );
      }
    }

    bb::msg::result_t OnProcessMessage(const bb::actor_t&, const bb::msg::basic_t& msg) override
    {
      if (auto keyEvent = bb::msg::As<bb::msg::keyEvent_t>(msg))
      {
        if (keyEvent->Press() != GLFW_RELEASE)
        {
          switch(keyEvent->Key())
          {
            case GLFW_KEY_UP:
              this->PrevLine();
              break;
            case GLFW_KEY_DOWN:
              this->NextLine();
              break;
            case GLFW_KEY_ENTER:
              this->SelectLine();
              break;
          }
        }
        return bb::msg::result_t::complete;
      }

      if (auto menuAction = bb::msg::As<menu::action_t>(msg))
      {
        switch(menuAction->ID())
        {
          case menu::action_t::id_t::prev:
            this->PrevLine();
            break;
          case menu::action_t::id_t::next:
            this->NextLine();
            break;
          case menu::action_t::id_t::select:
            this->SelectLine();
            break;
          default:
            BB_PANIC();
        }
        return bb::msg::result_t::complete;
      }

      if (auto done = bb::msg::As<menu::done_t>(msg))
      {
        this->selected = done->Selected();
        return bb::msg::result_t::complete;
      }

      BB_PANIC();
      return bb::msg::result_t::error;
    }

    const char* DefaultName() const override
    {
      return "menu_mdl";
    }

  public:

    model_t()
    : selected(ML_FIRST)
    {
      ;
    }

  };

}


int main(int argc, char* argv[])
{
  if (bb::ProcessStartupArguments(argc, argv) != 0)
  {
    return -1;
  }

  auto& context = bb::context_t::Instance();
  auto& pool = bb::workerPool_t::Instance();
  auto menuActor = pool.Register<menu::model_t>();

  context.RegisterActorCallback(menuActor, bb::context_t::keyboard);

  auto renderProgram = bb::shader_t::LoadProgramFromFiles(
    "007menu_vp.glsl",
    "007menu_fp.glsl"
  );

  auto font = bb::font_t("mono.config");
  auto text = bb::textDynamic_t(font, bb::vec2_t(0.1, 0.3));
  text.Update("%s", menuTextLines[0]);

  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

  auto lastTick = glfwGetTime();

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  bool run = true;

  bb::msg_t renderMsg; 

  do
  {
    while(renderTasks->Poll(&renderMsg))
    {
      if (auto anim = bb::As<menu::animation_t>(renderMsg))
      {
        auto line = anim->NewLine();
        text.Update("%s", menuTextLines[line]);
        pool.PostMessage(
          menuActor,
          bb::Issue<menu::done_t>(line)
        );
        continue;
      }

      if (bb::As<menu::exit_t>(renderMsg))
      {
        run = false;
        continue;
      }

      if (auto* defMsg = renderMsg.get())
      {
        bb::Error("Unknown message type: %s", typeid(*defMsg).name());
      }
      else
      {
        bb::Error("Unknown message type: %s", "Invalid Message");
      }
      BB_PANIC();
    }

    auto nowTick = glfwGetTime();
    auto delta = nowTick - lastTick;
    lastTick = nowTick;

    bb::framebuffer_t::Bind(context.Canvas());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    bb::shader_t::Bind(renderProgram);
    text.Render();

    context.Title(std::to_string(1.0/delta));

  } while (run && context.Update());

  pool.Unregister(menuActor);

  return 0;
}