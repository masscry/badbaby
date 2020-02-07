#include <context.hpp>
#include <arena.hpp>
#include <camera.hpp>

#include <vector>

namespace 
{

  bb::vbo_t CreateCircle(int sides, float radius)
  {
    std::vector<bb::vec2_t> points;

    if ((sides < 4) || (radius == 0.0f))
    { // some programmers error!
      assert(0);
      sides = 4;
      radius = 1.0f;
    }

    points.reserve(sides);

    float angleStep = static_cast<float>(M_PI*2.0/sides);
    float angle = 0.0f;
    while(sides-->0)
    {
      bb::vec2_t point;
      sincosf(angle, &point.x, &point.y);
      point *= radius;
      points.push_back(point);
      angle += angleStep;
    }
    return bb::vbo_t::CreateArrayBuffer(points.data(), sizeof(bb::vec2_t)*points.size(), false);
  }

}

namespace sub3000
{

  void arenaScene_t::OnPrepare()
  {
    bb::config_t menuConfig;
    menuConfig.Load("./arena.config");

    this->pContext = &bb::context_t::Instance();
    this->shader = bb::shader_t::LoadProgramFromFiles(
      menuConfig.Value("shader.vp", "arena.vp.glsl").c_str(),
      menuConfig.Value("shader.fp", "arena.fp.glsl").c_str()
    );

    auto circle = CreateCircle(
      static_cast<int>(menuConfig.Value("circle.sides", 16.0)),
      static_cast<float>(menuConfig.Value("circle.radius", 0.5))
    );

    this->radar = bb::vao_t::CreateVertexAttribObject();
    this->radar.BindVBO(circle, 0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    this->totalRadarVertecies = static_cast<int>(menuConfig.Value("circle.sides", 16.0));

    this->radarFrame = bb::framebuffer_t(512, 512);
  }

  void arenaScene_t::OnUpdate(double)
  {

  }
  
  void arenaScene_t::OnRender()
  {
    bb::framebuffer_t::Bind(this->radarFrame);
    bb::shader_t::Bind(this->shader);
    bb::vao_t::Bind(this->radar);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLineWidth(10.0f);

    glEnableVertexAttribArray(0);
    glDrawArrays(GL_LINE_LOOP, 0, this->totalRadarVertecies);
    glDisableVertexAttribArray(0);

    

  }

  void arenaScene_t::OnCleanup()
  {
    this->pContext = nullptr;
  }

  arenaScene_t::arenaScene_t()
  : scene_t(sceneID_t::arena, "Arena"),
    pContext(nullptr)
  {
    ;
  }

  arenaScene_t::~arenaScene_t()
  {

  }







}