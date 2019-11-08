#include <random>
#include <memory>
#include <cmath>

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <common.hpp>
#include <context.hpp>

#include <actor.hpp>
#include <worker.hpp>

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

enum menuMsg_t
{
  MT_FIRST = 0,
  MT_PREV = MT_FIRST,
  MT_NEXT,
  MT_ANIMATION_START,
  MT_ANIMATION_DONE,
  MT_LAST = MT_ANIMATION_DONE,
  MT_TOTAL
};

bb::mailbox_t renderTasks;

class menuModel_t: public bb::actor_t
{
  int selected;

  void ProcessMessage(bb::msg_t msg) override
  {
    int nextMenuLine = this->selected;

    switch (msg.type)
    {
      case MT_PREV:
        nextMenuLine = (((nextMenuLine-1) < ML_FIRST)?(ML_LAST):(nextMenuLine-1));
        break;
      case MT_NEXT:
        nextMenuLine = (((nextMenuLine+1) > ML_LAST)?(ML_FIRST):(nextMenuLine+1));
        break;
      case MT_ANIMATION_DONE:
        break;
      default:
        assert(0);
    }
  }

public:

};

int main(int argc, char* argv[])
{
  if (bb::ProcessStartupArguments(argc, argv) != 0)
  {
    return -1;
  }

  auto& context = bb::context_t::Instance();
  auto& pool = bb::workerPool_t::Instance();
  int menuActor = pool.Register(std::unique_ptr<bb::actor_t>(new menuModel_t));

  auto renderProgram = bb::shader_t::LoadProgramFromFiles(
    "007menu_vp.glsl",
    "007menu_fp.glsl"
  );

  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

  auto lastTick = glfwGetTime();

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  for(;;)
  {
    auto nowTick = glfwGetTime();
    auto delta = nowTick - lastTick;
    lastTick = nowTick;

    bb::framebuffer_t::Bind(context.Canvas());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    bb::shader_t::Bind(renderProgram);

    if (!context.Update())
    {
      break;
    }
    context.Title(std::to_string(1.0/delta));
  }

  pool.Unregister(menuActor);

  return 0;
}