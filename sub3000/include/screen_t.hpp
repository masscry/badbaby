/**
 * @file screen_t.hpp
 * 
 * Radar screen
 * 
 */
#pragma once
#ifndef __SUB3000_ARENA_SCREEN_HEADER__
#define __SUB3000_ARENA_SCREEN_HEADER__

#include <shader.hpp>
#include <scene.hpp>
#include <vao.hpp>
#include <framebuffer.hpp>
#include <sub3000.hpp>
#include <state_t.hpp>
#include <shapes.hpp>
#include <camera.hpp>
#include <mailbox.hpp>

#include <vector>

namespace sub3000
{

  namespace radar
  {

    class screen_t final: public scene_t
    {
      bb::framebuffer_t fb;
      bb::shader_t shader;

      bb::camera_t radarCamera;
      bb::mesh_t radar;

      bb::camera_t camera;
      bb::mesh_t units;
      bb::mesh_t radarLine;
      bb::mailbox_t::shared_t box;
      bb::actorPID_t spaceActorID;

      bb::linePoints_t unitPoints;
      std::deque<float> unitLife;

      bb::mesh_t depthZ;
      bb::mesh_t rudder;
      bb::mesh_t engine;
      bb::mesh_t ballast;

      bb::mesh_t speedMultMesh;
      int speedMult;

      float depth;
      float pointSize;
      float worldScale;

      bool hasCollision;

      bool mayCollide;

      screen_t(const screen_t&) = delete;
      screen_t& operator=(const screen_t&) = delete;

      void OnPrepare() override;
      void OnUpdate(double delta) override;
      void OnRender() override;
      void OnCleanup() override;

    public:

      void UpdateUnits(bb::linePoints_t&& units, float radarAngle, double dt);
      void UpdateDepthRadar(const state_t& state);
      void UpdateRudder(const state_t& state);
      void UpdateEngine(const state_t& state);
      void UpdateBallast(const state_t& state);

      bb::framebuffer_t& Framebuffer();
      const bb::framebuffer_t& Framebuffer() const;

      screen_t(screen_t&&) = default;
      screen_t& operator=(screen_t&&) = default;
      screen_t();
      ~screen_t() override = default;

    };

    inline bb::framebuffer_t& screen_t::Framebuffer()
    {
      return this->fb;
    }

    inline const bb::framebuffer_t& screen_t::Framebuffer() const
    {
      return this->fb;
    }

  } // namespace radar

} // namespace sub3000

#endif /* __SUB3000_ARENA_SCREEN_HEADER__ */
