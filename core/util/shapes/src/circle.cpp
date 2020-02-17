#include <shapes.hpp>
#include <common.hpp>

#include <cstdio>
#include <cmath>
#include <utility>
#include <memory>
#include <limits>

#include <glm/glm.hpp>

namespace bb
{

  bb::mesh_t GenerateCircle(uint32_t sides, float radius, float width)
  {
    return GenerateMesh(DefineCircle(glm::vec3(0.0f), sides, radius, width));
  }

  meshDesc_t DefineCircle(glm::vec3 center, uint32_t sides, float radius, float width)
  {
    std::vector<glm::vec2> points;
    std::vector<float> distance;
    std::vector<uint16_t> indecies;

    sides = bb::CheckValueBounds(sides, static_cast<uint32_t>(3), static_cast<uint32_t>(std::numeric_limits<uint16_t>::max()/2));
    radius = bb::CheckValueBounds(radius, 0.0f, 1000.0f);
    width = bb::CheckValueBounds(width, 0.01f, radius/2.0f);

    if (sides*4 > std::numeric_limits<uint16_t>::max())
    { // can't be so many sides!
      assert(0);
      sides = std::numeric_limits<uint16_t>::max()/4;
    }

    distance.reserve(sides*2);
    points.reserve(sides*2);
    indecies.reserve(sides*2+2);

    float angle = 0.0f;
    const float angleStep = static_cast<float>(M_PI*2.0/sides);
    const float outerRing = radius + width/2.0f;
    const float innerRing = radius - width/2.0f;
    uint16_t index = 0;
    while(sides-->0)
    {
      glm::vec2 point;
      sincosf(angle, &point.x, &point.y);
      point.x += center.x;
      point.y += center.y;

      points.push_back(point * outerRing);
      points.push_back(point * innerRing);

      distance.push_back(0.0f);
      distance.push_back(1.0f);

      indecies.push_back(index++);
      indecies.push_back(index++);
      angle += angleStep;
    }
    indecies.push_back(0);
    indecies.push_back(1);

    bb::Debug(
      "Circle:\n\tPoints: %lu\n\tIndecies: %lu",
      points.size(),
      indecies.size()
    );

    meshDesc_t result;

    result.Buffers().emplace_back(MakeVertexBuffer(std::move(points)));
    result.Buffers().emplace_back(MakeVertexBuffer(std::move(distance)));
    result.Indecies() = std::move(indecies);
    result.SetDrawMode(GL_TRIANGLE_STRIP);
    return result;
  }

}
