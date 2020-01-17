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

enum menuMsg_t: uint16_t
{
  MT_FIRST = bb::msgID_t::USR00,
  MT_PREV = MT_FIRST,
  MT_NEXT,
  MT_SELECT,
  MT_ANIMATION_START,
  MT_ANIMATION_DONE,
  MT_EXIT_GAME,
  MT_LAST = MT_ANIMATION_DONE,
  MT_TOTAL
};

bb::mailbox_t renderTasks;

class menuModel_t: public bb::role_t
{
  int selected;

  bb::msgResult_t OnProcessMessage(const bb::actor_t&, bb::msg_t msg) override
  {
    int nextMenuLine = this->selected;

    switch (msg.type)
    {
      case bb::KEYBOARD:
        {
          auto keyEvent = bb::GetMsgData<bb::keyEvent_t>(msg);
          if (keyEvent.press != GLFW_RELEASE)
          {
            switch(keyEvent.key)
            {
              case GLFW_KEY_UP:
                goto DO_PREV;
              case GLFW_KEY_DOWN:
                goto DO_NEXT;
              case GLFW_KEY_ENTER:
                goto DO_SELECT;
            }
          }
        }
        break;
      case MT_PREV:
      DO_PREV:
        nextMenuLine = (((nextMenuLine-1) < ML_FIRST)?(ML_LAST):(nextMenuLine-1));
        renderTasks.Put(bb::MakeMsg(-1, MT_ANIMATION_START, nextMenuLine));
        break;
      case MT_NEXT:
      DO_NEXT:
        nextMenuLine = (((nextMenuLine+1) > ML_LAST)?(ML_FIRST):(nextMenuLine+1));
        renderTasks.Put(bb::MakeMsg(-1, MT_ANIMATION_START, nextMenuLine));
        break;
      case MT_SELECT:
      DO_SELECT:
        if (this->selected == ML_EXIT)
        {
          renderTasks.Put(bb::MakeMsg(-1, MT_EXIT_GAME, 0));
        }
        break;
      case MT_ANIMATION_DONE:
        this->selected = bb::GetMsgData<int>(msg);
        break;
      default:
        assert(0);
    }
    return bb::msgResult_t::complete;
  }

public:

  menuModel_t()
  :selected(ML_FIRST)
  {
    ;
  }

};

int main(int argc, char* argv[])
{
  if (bb::ProcessStartupArguments(argc, argv) != 0)
  {
    return -1;
  }

  auto& context = bb::context_t::Instance();
  auto& pool = bb::workerPool_t::Instance();
  int menuActor = pool.Register(std::unique_ptr<bb::role_t>(new menuModel_t));

  context.RegisterActorCallback(menuActor, bb::cmfKeyboard);

  auto renderProgram = bb::shader_t::LoadProgramFromFiles(
    "007menu_vp.glsl",
    "007menu_fp.glsl"
  );

  auto font = bb::font_t("mono.config");
  auto text = bb::textDynamic_t(font, bb::vec2_t(0.1, 0.3));
  text.Update(menuTextLines[0]);

  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

  auto lastTick = glfwGetTime();

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  bool run = true;

  bb::msg_t renderMsg; 

  do
  {
    while(renderTasks.Poll(&renderMsg))
    {
      switch (renderMsg.type)
      {
      case MT_ANIMATION_START:
      {
        auto msgData = bb::GetMsgData<int>(renderMsg);
        text.Update(menuTextLines[msgData]);
        pool.PostMessage(menuActor, bb::MakeMsg(-1, MT_ANIMATION_DONE, msgData));
        break;
      }
      case MT_EXIT_GAME:
        run = false;
        break;
      default:
        assert(0);
      }
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