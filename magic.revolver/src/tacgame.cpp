#include <tacwar.hpp>
#include <script.hpp>
#include <algebra.hpp>

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

  }

  void game_t::OnUpdate(double)
  {
    bb::shader_t::Bind(this->spriteShader);
  }

  void game_t::OnMouse(int, int)
  {
  }

  void game_t::OnAction(int)
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

    this->spriteShader.SetMatrix(
      "model",
      glm::mat4(1.0f)
    );
    this->spriteShader.SetFloat(
      "contrast",
      0.5f
    );
    this->spriteShader.SetFloat(
      "bright",
      0.5f
    );
    this->spriteShader.SetFloat(
      "radSel",
      0.0f
    );
    this->spriteShader.SetVector3f(
      "acol",
      glm::vec3(0.0f, 0.0f, 1.0f)
    );

    bb::texture_t::Bind(this->spriteTex);
  }

  void game_t::Prepare()
  {
    this->spriteShader = bb::shader_t::LoadProgramFromFiles(
      "sprite.vp.glsl", "sprite.fp.glsl"
    );

    this->camera = bb::camera_t::Orthogonal(
      -100.0f, 100.0f, 100.0f, -100.0f
    );

    this->spriteTex = bb::texture_t::LoadTGA("basic.tga");
    this->spriteTex.SetFilter(GL_LINEAR, GL_LINEAR);
    this->sprite = bb::GeneratePlane(
      glm::vec2(2.0f), glm::vec3(0.0f), glm::vec2(0.5f), false
    );
  }

  void game_t::Cleanup()
  {
    this->spriteShader = bb::shader_t();
    this->spriteTex = bb::texture_t();
  }

}