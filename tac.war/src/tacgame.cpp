#include <tacwar.hpp>

#include <cmath>

namespace tac
{

  glm::vec2 MouseInWorld(bb::camera_t& camera)
  {
    auto& context = bb::context_t::Instance();
    auto curPos = context.MousePos();

    curPos.y = context.Height() - curPos.y;

    auto wPos = glm::unProjectZO(
      glm::vec3(curPos, 1.0f),
      glm::mat4(1.0f),
      camera.Projection(),
      glm::vec4(
        0.0f, 0.0f, context.Width(), context.Height()
      )
    );

    return glm::vec2(wPos.x, wPos.y);
  }

  void game_t::OnClick()
  {
    switch(this->mode)
    {
    case gameMode_t::select:
      {
        auto mouse = MouseInWorld(this->camera);

        for (auto it = this->troop.begin(), e = this->troop.end(); it != e; ++it)
        {
          if (glm::length(it->pos - mouse) < 1.0f)
          {
            this->sel = it;
            return;
          }
        }

        this->mode = gameMode_t::move;
        this->oldPos = this->sel->pos;
        this->newPos = mouse;

        auto dir = mouse - this->sel->pos;
        this->unitDir = atan2(dir.y, dir.x) - M_PI_2;

        this->time = 0.0;
        this->fulltime = glm::distance(this->newPos, this->sel->pos)/2.0f;
        this->timeMult = 1.0;
      }
      break;
    case gameMode_t::move:
      {
        this->timeMult = 5.0f;
      }
      break;
    default:
      assert(0);
    }
  }

  void game_t::OnUpdate(double dt)
  {
    bb::shader_t::Bind(this->spriteShader);
    this->camera.Update();

    if (this->mode == gameMode_t::move)
    {
      if (this->time < this->fulltime)
      {
        this->time += dt*this->timeMult*2.0;
        this->sel->pos = glm::mix(this->oldPos, this->newPos, this->time/this->fulltime);
        this->sel->angle = this->unitDir + sinf(this->time*15.0f)*0.10f;
      }
      else
      {
        this->sel->angle = this->unitDir;
        this->mode = gameMode_t::select;
      }
    }
  }

  void game_t::OnAction(int action)
  {

  }

  void game_t::OnRender()
  {
    auto& context = bb::context_t::Instance();

    bb::framebuffer_t::Bind(context.Canvas());
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    bb::shader_t::Bind(this->spriteShader);
    this->spriteShader.SetBlock(
      "camera",
      this->camera.UniformBlock()
    );

    bb::texture_t::Bind(this->spriteTex);
    for (auto it = this->troop.begin(), e = this->troop.end(); it != e; ++it)
    {
      auto& trooper = *it;

      glm::mat4 model = glm::rotate(
        glm::translate(
          glm::mat4(1.0f),
          glm::vec3(trooper.pos, 0.0f)
        ),
        trooper.angle,
        glm::vec3(0.0f, 0.0f, 1.0f)
      );
      this->spriteShader.SetMatrix(
        "model",
        model
      );
      this->spriteShader.SetFloat(
        "contrast",
        (it == this->sel)?2.0f:1.0f
      );
      this->spriteShader.SetFloat(
        "bright",
        (it == this->sel)?0.5f:0.0f
      );
      this->spriteShader.SetFloat(
        "radSel",
        (it == this->sel)?1.0f:0.0f
      );
      this->sprite.Render();
    }

    // render destination
    if (this->mode == gameMode_t::move)
    {
      glm::mat4 model = glm::rotate(
        glm::translate(
          glm::mat4(1.0f),
          glm::vec3(this->newPos, 0.0f)
        ),
        this->unitDir,
        glm::vec3(0.0f, 0.0f, 1.0f)
      );
      this->spriteShader.SetMatrix(
        "model",
        model
      );
      this->spriteShader.SetFloat(
        "contrast",
        0.0f
      );
      this->spriteShader.SetFloat(
        "bright",
        0.0f
      );
      this->spriteShader.SetFloat(
        "radSel",
        0.0f
      );
      this->sprite.Render();
    }

  }

  void game_t::Prepare()
  {
    this->spriteShader = bb::shader_t::LoadProgramFromFiles(
      "sprite.vp.glsl", "sprite.fp.glsl"
    );

    auto aspect = bb::context_t::Instance().AspectRatio();
    this->camera = bb::camera_t::Orthogonal(
      0.0f, 40.0f*aspect, 40.0f, 0.0f
    );

    this->spriteTex = bb::texture_t::LoadTGA("basic.tga");
    this->sprite = bb::GeneratePlane(
      glm::vec2(2.0f), glm::vec3(0.0f), glm::vec2(0.5f), false
    );

    for (int i = 0; i < 8; ++i)
    {
      this->troop.emplace_back(
        trooper_t{
          { ((i%4)+1) * 4.2f, (i >= 4)?35.0f:38.0f },
          glm::radians(180.0f)
        }
      );
    }
    this->sel = this->troop.begin();
    this->mode = gameMode_t::select;
  }

  void game_t::Cleanup()
  {
    this->spriteShader = bb::shader_t();
    this->spriteTex = bb::texture_t();
  }

}