#include <status_t.hpp>

#include <player.hpp>
#include <space.hpp>

namespace sub3000
{

  namespace radar
  {

    void status_t::OnPrepare()
    {
      bb::config_t menuConfig;
      menuConfig.Load("./arena.config");

      this->mapDims.x = 64.0f;
      this->mapDims.y = 64.0f;

      this->camera = bb::camera_t::Orthogonal(
        0.0f, this->mapDims.x, this->mapDims.y, 0.0f
      );

      this->fb = bb::framebuffer_t(512, 512);
      this->box = bb::postOffice_t::Instance().New("arenaStatus");

      this->mapShader = bb::shader_t::LoadProgramFromFiles(
        menuConfig.Value("map.shader.vp", "hmap-debug.vp.glsl").c_str(),
        menuConfig.Value("map.shader.fp", "hmap-debug.fp.glsl").c_str()
      );
      
      this->mapPointsShader = bb::shader_t::LoadProgramFromFiles(
        menuConfig.Value("map.points.vp", "radar-2.vp.glsl").c_str(),
        menuConfig.Value("map.points.fp", "radar-2.fp.glsl").c_str()
      );

      this->mapPlane = bb::GeneratePlane(
        glm::vec2(512.0f, 256.0f),
        glm::vec3(0.0f),
        glm::vec2(0.0f, 0.0f),
        true
      );

      this->statCounter = 0;

    }

    namespace
    {

      const glm::vec2 shipTri[] = 
      {
        glm::vec2(-0.3f,   0.0f),
        glm::vec2( 0.3f,   0.0f),
        glm::vec2( 0.0f,  -1.0f),
        glm::vec2(-0.3f,   0.0f)
      };

      bb::meshDesc_t ShipTriangle(float pointSize, glm::vec2 pos, float angle)
      {
        auto points = bb::linePoints_t();

        auto tmap = 
            glm::rotate(
              glm::translate(
                glm::mat3(1.0f),
                pos
              ),
              angle
            );

        for (auto triPoint: shipTri)
        {
          points.emplace_back(
            tmap*glm::vec3(triPoint, 1.0f)
          );
        }

        return bb::DefineLine(glm::vec3(0.0f), pointSize * 0.02f, points);
      }

    }

    void status_t::OnUpdate(double)
    {
      bb::msg_t msg;
      if (this->box->Poll(&msg))
      {
        if (auto mapData = bb::As<bb::msg::dataMsg_t<bb::ext::heightMap_t>>(msg))
        {
          const auto& hmap = mapData->Data();

          this->mapTex = bb::texture_t(
            hmap.Width(),
            hmap.Height(),
            hmap.Data()
          );
          this->mapTex.SetFilter(
            GL_LINEAR, GL_LINEAR
          );
        }

        if (auto status = bb::As<player::status_t>(msg))
        {
          camera.View() = glm::translate(
            glm::mat4(1.0f),
            glm::vec3(-status->Data().pos + this->mapDims/2.0f, 0.0f)
          );

          this->mapShip = bb::GenerateMesh(ShipTriangle(10.0f, status->Data().pos, status->Data().angle));

          ++this->statCounter;
          if ((this->statCounter > 30))
          {
            if ((this->coursePoints.empty()) || (glm::length(this->coursePoints.back() - status->Data().pos) >= 0.4f))
            {
              this->statCounter = 0;
              this->coursePoints.emplace_back(
                status->Data().pos
              );

              if (this->coursePoints.size() > 200)
              {
                this->coursePoints.pop_front();
              }

              this->mapPoints = bb::GenerateMesh(
                bb::DefinePoints(0.2f, this->coursePoints)
              );
            }
          }
        }
      }
    }

    void status_t::OnRender()
    {
      bb::framebuffer_t::Bind(this->fb);

      glBlendFunc(
        GL_ONE, GL_ONE_MINUS_SRC_ALPHA
      );

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      camera.Update();

      bb::shader_t::Bind(this->mapShader);
      this->mapShader.SetBlock(
        this->mapShader.UniformBlockIndex("camera"),
        this->camera.UniformBlock()
      );
      this->mapShader.SetFloat("border", 28.0f/63.0f);

      bb::texture_t::Bind(this->mapTex);
      this->mapPlane.Render();

      bb::shader_t::Bind(this->mapPointsShader);
      this->mapPointsShader.SetBlock(
        this->mapPointsShader.UniformBlockIndex("camera"),
        this->camera.UniformBlock()
      );
      if (this->mapPoints.Good())
      {
        this->mapPoints.Render();
      }
      if (this->mapShip.Good())
      {
        this->mapShip.Render();
      }
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