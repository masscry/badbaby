#include <tacwar.hpp>
#include <script.hpp>
#include <algebra.hpp>

#include <cmath>

class levelVM_t final: public bb::vm_t
{
  tac::game_t::troop_t troops;
  bb::linePoints_t level;
  int curTeam;

  int OnCommand(int cmd, const bb::listOfRefs_t& refs) override
  {
    switch(cmd)
    {
      case 'u': // place unit
      {
        float x = static_cast<float>(bb::Argument(refs, 0));
        float y = static_cast<float>(bb::Argument(refs, 1));
        float angle = static_cast<float>(bb::Argument(refs, 2));

        this->troops.emplace_back(
          tac::trooper_t{
            { x, y },
            glm::radians(angle),
            this->curTeam,
            0,
            8.0f
          }
        );
        break;
      }
      case 't':
      {
        this->curTeam = static_cast<int>(bb::Argument(refs, 0));
        if ((this->curTeam > tac::TEAM_TOTAL) || (this->curTeam < tac::TEAM_FIRST))
        {
          this->curTeam = tac::TEAM_ALICE;
        }
        break;
      }
      case 'p':
      {
        this->level.emplace_back(
          glm::vec2(
            bb::Argument(refs, 0),
            bb::Argument(refs, 1)
          )
        );
      }
      default:
        bb::Debug("Command %c (%d)\n", cmd, cmd);
        for (auto& item: refs)
        {
          bb::Debug("\t%f\n", item.Number());
        }
    }
    return 0;
  }

public:

  bb::linePoints_t& Level()
  {
    return this->level;
  }

  const bb::linePoints_t& Level() const
  {
    return this->level;
  }

  tac::game_t::troop_t& Troops()
  {
    return this->troops;
  }

  const tac::game_t::troop_t& Troops() const
  {
    return this->troops;
  }

  levelVM_t()
  : curTeam(-1)
  {
    ;
  }

  ~levelVM_t() override
  {
    ;
  }

  levelVM_t(const levelVM_t&) = delete;
  levelVM_t& operator=(const levelVM_t&) = delete;
  levelVM_t(levelVM_t&&) = delete;
  levelVM_t& operator=(levelVM_t&&) = delete;

};

namespace tac
{

  float cross2d(glm::vec2 v, glm::vec2 w)
  {
    return v.x*w.y - v.y*w.x;
  }

  template<typename t1, typename t2, typename t3>
  bool between(t1 a, t2 val, t3 b)
  {
    return (a <= val) && (val <= b);
  }

  bool SegmentSegmentIntersect(tac::segment_t l1, tac::segment_t l2, glm::vec2* point)
  {
    auto d1 = l1.start - l1.finish;
    auto d2 = l1.start - l2.start;
    auto d3 = l2.start - l2.finish;

    auto dt = d1.x * d3.y - d1.y * d3.x;

    if (dt == 0.0f)
    {
      return false;
    }

    auto t = (d2.x * d3.y - d2.y * d3.x) / dt;
    auto u = - (d1.x * d2.y - d1.y * d2.x) / dt;

    auto ist = (t >= 0.0f) && (t <= 1.0f);
    auto isu = (u >= 0.0f) && (u <= 1.0f);

    if ((ist && isu) && (point != nullptr))
    {
      *point = l1.start + t * (l1.finish - l1.start);
    }
    return ist && isu;
  }

  bool SegmentLevelIntersect(tac::segment_t line, const tac::game_t::segments_t& level, glm::vec2* point)
  {
    //
    // TODO: make faster algo!
    //
    auto result = false;
    auto nearestPoint = line.finish;

    for (auto seg: level)
    {
      glm::vec2 isec;
      if (SegmentSegmentIntersect(line, seg, &isec))
      {
        result = true;
        if (point == NULL)
        { // Do not calculate further, any intersection enough
          return result;
        }
        if (glm::distance(line.start, isec) < glm::distance(line.start, nearestPoint))
        {
          nearestPoint = isec;
        }
      }
    }
    if (result)
    {
      if (point != NULL)
      {
        *point = nearestPoint;
      }
    }
    return result;
  }

  bb::linePoints_t BuildSemCircle(const glm::mat4& model, const tac::game_t::segments_t& level)
  {
    bb::linePoints_t semCircle;

    auto invModel = glm::inverse(model);

    for (float angle = -40.0f; angle < 40.0f; angle += 1.0f)
    {

      auto radAngle = glm::radians(angle);

      auto dirAngle = bb::Dir(radAngle);

      auto rayStart = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
      auto rayFinish = glm::vec4(dirAngle*10.0f, 0.0f, 1.0f);

      rayStart = model * rayStart;
      rayFinish = model * rayFinish;

      auto ray = tac::segment_t{
        glm::vec2(rayStart),
        glm::vec2(rayFinish)
      };

      glm::vec2 isec;

      if (!SegmentLevelIntersect(ray, level, &isec))
      {
        isec = ray.finish;
      }

      bb::context_t::Instance().Title(
        std::to_string(rayStart.x) 
        + " " + std::to_string(rayStart.y)
        + " " + std::to_string(isec.x) 
        + " " + std::to_string(isec.y)
      );

      semCircle.emplace_back(
        glm::vec4(dirAngle, 0.0f, 1.0f)
      );

      semCircle.emplace_back(
        invModel * glm::vec4(isec, 0.0f, 1.0f)
      );

    }

    return semCircle;
  }

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
          if ((this->sel != it) && (glm::length(it->pos - mouse) < 1.0f) && (it->team == this->curTeam) && (it->flags == 0))
          {
            this->sel = it;
            return;
          }
        }

        if (!this->showShade)
        {
          return;
        }

        this->oldPos = this->sel->pos;

        if (glm::distance(mouse, this->sel->pos) <= this->sel->maxDist)
        {
          this->newPos = mouse;
          auto dir = this->newPos - this->sel->pos;
          this->finalDir = atan2(dir.y, dir.x) - M_PI_2;
        }
        else
        {
          auto dir = glm::normalize(mouse - this->sel->pos);
          this->finalDir = atan2(dir.y, dir.x) - M_PI_2;
          this->newPos = this->sel->pos + dir * this->sel->maxDist;
        }

        this->mode = gameMode_t::dir;

        this->fulltime = glm::distance(this->newPos, this->sel->pos)/2.0f;
        this->timeMult = 1.0;

        auto dir = mouse - this->sel->pos;
        this->unitDir = atan2(dir.y, dir.x) - M_PI_2;
      }
      break;
    case gameMode_t::dir:
      {
        auto mouse = MouseInWorld(this->camera);
        auto dir = mouse - this->newPos;
        if (glm::length(dir) == 0.0)
        {
          dir = this->newPos - this->oldPos;
        }
        this->finalDir = atan2(dir.y, dir.x) - M_PI_2;
        this->mode = gameMode_t::move;

        this->time = 0.0;
        this->timeMult = 1.0;
        *this->sel->lineMesh = bb::mesh_t();
      }
      break;
    case gameMode_t::move:
    case gameMode_t::rot:
      {
        this->timeMult = 5.0f;
      }
      break;
    default:
      assert(0);
    }
  }

  float MinDistToSeg(glm::vec2 v, glm::vec2 w, glm::vec2 p) 
  {
    auto l2 = glm::distance(v, w);
    l2*=l2;
    if (l2 == 0.0f)
    {
      return glm::distance(p, v);
    }
    auto t = glm::max(0.0f, glm::min(1.0f, glm::dot(p - v, w - v) / l2));
    auto projection = v + t * (w - v);

    return glm::distance(p, projection);
  }

  float MinDistToLevel(const tac::game_t::segments_t& level, glm::vec2 point)
  {
    auto minDist = std::numeric_limits<float>::max();
    for (const auto& seg: level)
    {
      auto minDistToSeg = MinDistToSeg(seg.start, seg.finish, point);
      minDist = (minDistToSeg < minDist)?minDistToSeg:minDist;
    }
    return minDist;
  }

  void game_t::OnUpdate(double dt)
  {
    bb::shader_t::Bind(this->spriteShader);
    this->camera.Update();

    switch (this->mode)
    {
    case gameMode_t::select:
      if (this->sel->team != this->curTeam)
      {
        for (auto it = this->troop.begin(), e = this->troop.end(); it != e; ++it)
        {
          if ((it->team == this->curTeam) && (it->flags == 0))
          {
            this->sel = it;
            return;
          }
        }

        // here all units made their moves
        for (auto it = this->troop.begin(), e = this->troop.end(); it != e; ++it)
        {
          it->flags = 0;
        }
        ++this->round;
      }
      else
      {
        auto mouse = MouseInWorld(this->camera);
        this->showShade = true;

        glm::vec2 newUnitPos;
        auto dir = glm::normalize(mouse - this->sel->pos);
        auto newFinalDir = atan2(dir.y, dir.x) - M_PI_2;

        if (glm::distance(mouse, this->sel->pos) <= this->sel->maxDist)
        {
          newUnitPos = mouse;
        }
        else
        {
          newUnitPos = this->sel->pos + dir * this->sel->maxDist;
        }

        for (auto it = this->troop.begin(), e = this->troop.end(); it != e; ++it)
        {
          if ((glm::length(it->pos - newUnitPos) < 1.0f) && (it != this->sel))
          {
            this->showShade = false;
            break;
          }
        }

        if (!this->showShade)
        {
          break;
        }

        auto minDistToLevel = MinDistToLevel(this->segments, newUnitPos);
        if (minDistToLevel <= 1.0f)
        {
          this->showShade = false;
        }

        if (!this->showShade)
        {
          break;
        }

        auto segLine = segment_t {
          this->sel->pos,
          newUnitPos
        };

        if (SegmentLevelIntersect(segLine, this->segments, NULL))
        {
          this->showShade = false;
        }

        if (this->showShade)
        {
          this->finalDir = newFinalDir;
          this->newPos = newUnitPos;
          this->shadeChanged = true;
        }
      }
      break;
    case gameMode_t::dir:
      {
        this->showShade = true;
        auto mouse = MouseInWorld(this->camera);
        auto dir = mouse - this->newPos;
        if (glm::length(dir) == 0.0)
        {
          dir = this->newPos - this->oldPos;
        }
        auto newDir = atan2(dir.y, dir.x) - M_PI_2;
        if (newDir != this->finalDir)
        {
          this->finalDir = newDir;
          this->shadeChanged = true;
        }
      }
      break;
    case gameMode_t::move:
      if (this->time < this->fulltime)
      {
        this->showShade = true;
        this->time = glm::min(this->time + dt*this->timeMult*2.0, this->fulltime);
        this->sel->pos = glm::mix(this->oldPos, this->newPos, this->time/this->fulltime);
        this->sel->angle = this->unitDir + sinf(this->time*15.0f)*0.10f;
      }
      else
      {
        this->showShade = false;
        this->fulltime = 1.0;
        this->time = 0.0;
        this->mode = gameMode_t::rot;
      }
      break;
    case gameMode_t::rot:
      this->showShade = false;
      if (this->time < this->fulltime)
      {
        this->time = glm::min(this->time + dt*this->timeMult*5.0, this->fulltime);
        this->sel->angle = glm::mix(this->unitDir, this->finalDir, this->time/this->fulltime);
      }
      else
      {
        ++this->curTeam;
        if (this->curTeam >= tac::TEAM_TOTAL)
        {
          this->curTeam = tac::TEAM_ALICE;
        }
        this->mode = gameMode_t::select;
        this->sel->angle = this->finalDir;
        this->sel->flags = 1;
        *this->sel->lineMesh = std::move(this->lineMesh);
        this->timeMult = 1.0;
      }
      break;
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
    this->level.Render();

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
      this->spriteShader.SetVector3f(
        "acol",
        teamColor[it->team]
      );
      this->sprite.Render();
      if (trooper.lineMesh->Good())
      {
        trooper.lineMesh->Render();
      }
    }

    // render destination
    if (this->showShade)
    {
      glm::mat4 model = glm::rotate(
        glm::translate(
          glm::mat4(1.0f),
          glm::vec3(this->newPos, 0.0f)
        ),
        this->finalDir,
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

      if (this->shadeChanged)
      {
        auto semCircle = BuildSemCircle(model, this->segments);
        auto meshDesc = bb::DefineTriangleStrip(semCircle);
        this->lineMesh = bb::GenerateMesh(meshDesc);
        this->shadeChanged = false;
      }
      this->lineMesh.Render();
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
    this->spriteTex.SetFilter(GL_LINEAR, GL_LINEAR);
    this->sprite = bb::GeneratePlane(
      glm::vec2(2.0f), glm::vec3(0.0f), glm::vec2(0.5f), false
    );

    levelVM_t levelVM;
    bb::ExecuteScriptFile(levelVM, "level.txt");

    this->troop = std::move(levelVM.Troops());
    this->sel = this->troop.begin();
    this->mode = gameMode_t::select;
    this->troopLineMesh.resize(this->troop.size());

    for (size_t it = 0, e = this->troop.size(); it < e; ++it)
    {
      this->troop[it].lineMesh = &this->troopLineMesh[it];
    }

    this->curTeam = tac::TEAM_ALICE;
    this->round = 0;
    this->shadeChanged = true;

    if (levelVM.Level().size() >= 2)
    {
      this->level = bb::GenerateLine(0.1f, levelVM.Level());

      for (
        auto cur = levelVM.Level().begin(),
            next = cur + 1,
            last = levelVM.Level().end();
        next != last;
        ++cur, ++next
      )
      {
        this->segments.emplace_back(
          segment_t{ *cur, *next }
        );
      }
    }

    for (auto& it: this->troop)
    {
      glm::mat4 model = glm::rotate(
        glm::translate(
          glm::mat4(1.0f),
          glm::vec3(it.pos, 0.0f)
        ),
        it.angle,
        glm::vec3(0.0f, 0.0f, 1.0f)
      );

      auto semCircle = BuildSemCircle(model, this->segments);
      auto meshDesc = bb::DefineTriangleStrip(semCircle);
      *it.lineMesh = bb::GenerateMesh(meshDesc);
    }

  }

  void game_t::Cleanup()
  {
    this->spriteShader = bb::shader_t();
    this->spriteTex = bb::texture_t();
  }

}