#include <shapes.hpp>
#include <common.hpp>

#include <cstdio>
#include <cstring>
#include <cmath>
#include <utility>
#include <memory>
#include <limits>

namespace bb
{


  meshDesc_t DefineTriangleStrip(const linePoints_t& tripoints)
  {
    std::vector<glm::vec2> points;
    std::vector<glm::vec2> distance;
    std::vector<uint16_t> indecies;

    if (tripoints.size() < 3)
    { // Programmer's error!
      // Can't be so few or so many points
      BB_PANIC();
      return bb::meshDesc_t();
    }

    points.reserve(tripoints.size());
    distance.reserve(tripoints.size());
    indecies.reserve(tripoints.size());

    uint16_t index = 0;

    for (const auto& point: tripoints)
    {
      points.emplace_back(point);
      distance.emplace_back(glm::vec2(0.18f,0.5f));
      indecies.emplace_back(index++);
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


}