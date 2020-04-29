#include <status_t.hpp>

#include <player.hpp>

namespace sub3000
{

  namespace radar
  {

    void status_t::OnPrepare()
    {
      bb::config_t menuConfig;
      menuConfig.Load("./arena.config");

      this->shader = bb::shader_t::LoadProgramFromFiles(
        menuConfig.Value("status.shader.vp", "status.vp.glsl").c_str(),
        menuConfig.Value("status.shader.fp", "status.fp.glsl").c_str()
      );

      this->camera = bb::camera_t::Orthogonal(
        -12.0f, 500.0f, -256.0f, 256.0f
      );

      this->fb = bb::framebuffer_t(512, 512);
      this->box = bb::postOffice_t::Instance().New("arenaStatus");

      this->font = bb::font_t(menuConfig.Value("status.font", "mono.config"));
      this->text = bb::textDynamic_t(this->font, bb::vec2_t(12.0f, -24.0f));
    }

    void status_t::OnUpdate(double)
    {
      bb::msg_t msg;
      if (this->box->Poll(&msg))
      {
        if (auto status = bb::As<player::status_t>(msg))
        {
          this->text.Update(
            "POS:\t[%+6.3f;%+6.3f]\n"
            "DEPTH:\t%+6.3f\n"
            "ENGINE:\t%s\n"
            "OUTPUT:\t%+6.3f\n"
            "SPEED:\t[%+6.3f;%+6.3f]\n"
            "RUDDER:\t%s [%+6.3f]\n"
            "AVEL:\t%+6.3f\n"
            "ANGLE:\t%+6.3f\n"
            "DRAG:\t%+6.3f",
            status->Data().pos.x, status->Data().pos.y,
            status->Data().depth,
            engine::ToString(status->Data().engine),
            status->Data().engineOutput,
            status->Data().vel.x, status->Data().vel.y,
            rudder::ToString(status->Data().rudder), status->Data().rudderPos*180.0/M_PI,
            status->Data().aVel*180.0/M_PI,
            status->Data().angle*180.0/M_PI,
            status->Data().dragCoeff
          );
        }
      }
    }

    void status_t::OnRender()
    {
      bb::framebuffer_t::Bind(this->fb);
      
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      bb::shader_t::Bind(this->shader);
      camera.Update();

      this->shader.SetBlock(
        this->shader.UniformBlockIndex("camera"),
        this->camera.UniformBlock()
      );

      this->shader.SetVector3f(
        "glyphColor", glm::vec3(0.2f, 0.7f, 0.0f)
      );
      this->shader.SetMatrix(
        "model",
        glm::mat4(1.0f)
      );
      this->text.Render();
    }

    void status_t::OnCleanup()
    {
      this->box.reset();
    }

    bb::framebuffer_t& status_t::Framebuffer()
    {
      return this->fb;
    }

    const bb::framebuffer_t& status_t::Framebuffer() const
    {
      return this->fb;
    }

    status_t::status_t()
    : scene_t(sceneID_t::radarStatus, "Status")
    {
      ;
    }

  }


}