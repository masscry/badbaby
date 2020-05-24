/**
 * @file demo.hpp
 * 
 * Control demo scheme.
 */

#include <screen_t.hpp>
#include <status_t.hpp>

namespace sub3000
{

  class demoScene_t final: public scene_t
  {
    bb::mailbox_t::shared_t box;

    bb::camera_t   camera;
    bb::shader_t   shader;
    bb::mesh_t     mesh;

    bb::mesh_t     lines;

    bb::actorPID_t space;

    void OnPrepare() override;
    void OnUpdate(double delta) override;
    void OnRender() override;
    void OnCleanup() override;

  public:

    demoScene_t();
    ~demoScene_t() override = default;

  };

} // namespace sub3000
