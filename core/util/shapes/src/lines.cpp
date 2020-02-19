#include <shapes.hpp>
#include <common.hpp>
#include <context.hpp>

#include <cstdio>
#include <cmath>
#include <utility>
#include <memory>
#include <limits>

#include <glm/glm.hpp>

namespace bb
{

  bb::mesh_t GenerateLine(float width, const linePoints_t& linePoints)
  {
    return GenerateMesh(DefineLine(glm::vec3(0.0f), width, linePoints));
  }

  meshDesc_t DefineLine(glm::vec3 offset, float width, const linePoints_t& linePoints)
  {
    std::vector<glm::vec2> points;
    std::vector<float> distance;
    std::vector<uint16_t> indecies;

    width = bb::CheckValueBounds(width, 0.01f, 100.0f);

    if ((linePoints.size() < 2) || (linePoints.size()*2 > std::numeric_limits<uint16_t>::max()))
    { // Programmer's error!
      // Can't be so few or so many points
      assert(0);
      return bb::meshDesc_t();
    }

    points.reserve(linePoints.size()*2);
    distance.reserve(linePoints.size()*2);
    indecies.reserve(linePoints.size()*3);

    uint16_t index = 0;
    auto off2d = glm::vec2(offset.x, offset.y);
    auto item = linePoints.begin();
    auto end = linePoints.end();
    while (std::next(item) != end)
    {
      auto nextItem = std::next(item);

      auto dir = glm::normalize(*nextItem - *item);
      auto tangent = glm::vec2(dir.y, -dir.x)*width/2.0f;

      points.push_back(*item + tangent + off2d);
      points.push_back(*item - tangent + off2d);
      points.push_back(*nextItem + tangent + off2d);
      points.push_back(*nextItem - tangent + off2d);

      distance.emplace_back(0.0f);
      distance.emplace_back(1.0f);
      distance.emplace_back(0.0f);
      distance.emplace_back(1.0f);

      indecies.push_back(index++);
      indecies.push_back(index++);
      indecies.push_back(index++);
      indecies.push_back(index++);

      item = nextItem;
      ++nextItem;
      if (nextItem != end)
      {
        indecies.push_back(BREAKING_INDEX);
      }
    }

    bb::meshDesc_t result;

    result.Buffers().emplace_back(
      MakeVertexBuffer(std::move(points))
    );
    result.Buffers().emplace_back(
      MakeVertexBuffer(std::move(distance))
    );
    result.Indecies() = std::move(indecies);
    result.SetDrawMode(GL_TRIANGLE_STRIP);
    return result;
  }

  namespace
  {
    //
    // 1---2
    // |\ /|
    // | 0 |
    // |/ \|
    // 4---3
    //
    const uint16_t pointIndecies[6] = {
      0, 1, 2, 3, 4, 1
    };
  }

  meshDesc_t DefinePoints(float width, const linePoints_t& points)
  {
    std::vector<glm::vec2> vpos;
    std::vector<float> distance;
    std::vector<uint16_t> indecies;

    width = bb::CheckValueBounds(width, 0.01f, 100.0f);

    size_t totalPoints = points.size();

    if ((totalPoints == 0) || (totalPoints*5 >= std::numeric_limits<uint16_t>::max()))
    { // Programmer's error!
      // Can't be so few or so many points
      assert(0);
      return bb::meshDesc_t();
    }

    vpos.reserve(points.size()*5);
    distance.reserve(points.size()*5);
    indecies.reserve(points.size()*7);

    for (auto cpos: points)
    {
      uint16_t cIndex = static_cast<uint16_t>(vpos.size());

      vpos.emplace_back(cpos);
      vpos.emplace_back(cpos.x - width, cpos.y - width);
      vpos.emplace_back(cpos.x + width, cpos.y - width);
      vpos.emplace_back(cpos.x + width, cpos.y + width);
      vpos.emplace_back(cpos.x - width, cpos.y + width);

      distance.emplace_back(1.0f);
      distance.emplace_back(0.0f);
      distance.emplace_back(0.0f);
      distance.emplace_back(0.0f);
      distance.emplace_back(0.0f);

      for (size_t i = 0; i < countof(pointIndecies); ++i)
      {
        indecies.emplace_back(cIndex + pointIndecies[i]);
      }
      indecies.emplace_back(0xFFFF);
    }

    bb::meshDesc_t result;

    result.Buffers().emplace_back(
      MakeVertexBuffer(std::move(vpos))
    );
    result.Buffers().emplace_back(
      MakeVertexBuffer(std::move(distance))
    );
    result.Indecies() = std::move(indecies);
    result.SetDrawMode(GL_TRIANGLE_FAN);
    return result;
  }

}