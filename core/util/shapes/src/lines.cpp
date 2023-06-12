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

  namespace
  {
    //
    // 0---1
    // |  /|
    // | / |
    // |/  |
    // 3---2
    //
    const uint16_t pointIndecies[] = 
    {
      0, 1, 3, 2
    };

    const glm::vec2 topleft(0.0f, 0.0f);
    const glm::vec2 topright(0.0f, 1.0f);
    const glm::vec2 left(0.5f, 0.0f);
    const glm::vec2 right(0.5f, 1.0f);
    const glm::vec2 bottomleft(1.0f, 0.0f);
    const glm::vec2 bottomright(1.0f, 1.0f);

  }

  bb::mesh_t GenerateLine(float width, const linePoints_t& linePoints)
  {
    return GenerateMesh(DefineLine(glm::vec3(0.0f), width, linePoints));
  }

  meshDesc_t DefineLine(glm::vec3 offset, float width, const linePoints_t& linePoints)
  {
    std::vector<glm::vec2> points;
    std::vector<glm::vec2> distance;
    std::vector<uint16_t> indecies;
    auto breakIndex = bb::breakingIndex<uint16_t>();

    width = bb::CheckValueBounds(width, 0.01f, 100.0f);

    if ((linePoints.size() < 2) || (linePoints.size()*2 > std::numeric_limits<uint16_t>::max()))
    { // Programmer's error!
      // Can't be so few or so many points
      BB_PANIC();
      return bb::meshDesc_t();
    }

    points.reserve((linePoints.size()-1)*8);
    distance.reserve((linePoints.size()-1)*8);
    indecies.reserve((linePoints.size()-1)*9);

    uint16_t index = 0;
    auto off2d = glm::vec2(offset.x, offset.y);
    auto item = linePoints.begin();
    auto end = linePoints.end();

    while (std::next(item) != end)
    {
      auto nextItem = std::next(item);

      auto dir = glm::normalize(*nextItem - *item);
      auto tangent = glm::vec2(dir.y, -dir.x)*width/2.0f;

      points.emplace_back(*item + tangent + off2d - dir*width/2.0f);
      points.emplace_back(*item - tangent + off2d - dir*width/2.0f);
      points.emplace_back(*item + tangent + off2d + dir*width/2.0f);
      points.emplace_back(*item - tangent + off2d + dir*width/2.0f);
      points.emplace_back(*nextItem + tangent + off2d - dir*width/2.0f);
      points.emplace_back(*nextItem - tangent + off2d - dir*width/2.0f);
      points.emplace_back(*nextItem + tangent + off2d + dir*width/2.0f);
      points.emplace_back(*nextItem - tangent + off2d + dir*width/2.0f);

      distance.emplace_back(topleft);
      distance.emplace_back(topright);
      distance.emplace_back(left);
      distance.emplace_back(right);
      distance.emplace_back(left);
      distance.emplace_back(right);
      distance.emplace_back(bottomleft);
      distance.emplace_back(bottomright);

      for (uint16_t cnt = 0; cnt < 8; ++cnt)
      {
        indecies.push_back(index + cnt);
      }
      index += 8;
      item = nextItem;
      ++nextItem;
      indecies.emplace_back(breakIndex);
    }

    bb::meshDesc_t result;

    result.Buffers().emplace_back(
      MakeVertexBuffer(std::move(points))
    );
    result.Buffers().emplace_back(
      MakeVertexBuffer(std::move(distance))
    );
    result.Indecies() = MakeIndexBuffer(std::move(indecies));
    result.SetDrawMode(GL_TRIANGLE_STRIP);
    return result;
  }

  meshDesc_t DefinePoints(float width, const linePoints_t& points)
  {
    std::vector<glm::vec2> vpos;
    std::vector<glm::vec2> distance;
    std::vector<uint16_t> indecies;
    const auto breakingIndex = bb::breakingIndex<uint16_t>();

    width = bb::CheckValueBounds(width, 0.01f, 100.0f);

    size_t totalPoints = points.size();

    if ((totalPoints == 0) || (totalPoints*9 >= std::numeric_limits<uint16_t>::max()))
    { // Programmer's error!
      // Can't be so few or so many points
      BB_PANIC();
      return bb::meshDesc_t();
    }

    vpos.reserve(points.size()*4);
    distance.reserve(points.size()*4);
    indecies.reserve(points.size()*5);

    for (auto cpos: points)
    {
      uint16_t cIndex = static_cast<uint16_t>(vpos.size());

      vpos.emplace_back(cpos.x - width, cpos.y - width);
      vpos.emplace_back(cpos.x + width, cpos.y - width);
      vpos.emplace_back(cpos.x + width, cpos.y + width);
      vpos.emplace_back(cpos.x - width, cpos.y + width);

      distance.emplace_back(topleft);
      distance.emplace_back(topright);
      distance.emplace_back(bottomright);
      distance.emplace_back(bottomleft);

      for (size_t i = 0; i < countof(pointIndecies); ++i)
      {
        indecies.emplace_back(cIndex + pointIndecies[i]);
      }
      indecies.emplace_back(breakingIndex);
    }

    bb::meshDesc_t result;

    result.Buffers().emplace_back(
      MakeVertexBuffer(std::move(vpos))
    );
    result.Buffers().emplace_back(
      MakeVertexBuffer(std::move(distance))
    );
    result.Indecies() = MakeIndexBuffer(std::move(indecies));
    result.SetDrawMode(GL_TRIANGLE_STRIP);
    return result;
  }

}
