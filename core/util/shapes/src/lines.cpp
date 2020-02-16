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

  bb::mesh_t GenerateLine(float width, const linePoints_t& linePoints)
  {
    std::vector<glm::vec2> points;
    std::vector<float> distance;
    std::vector<uint16_t> indecies;

    width = bb::CheckValueBounds(width, 0.01f, 100.0f);

    if ((linePoints.size() < 2) || (linePoints.size()*2 > std::numeric_limits<uint16_t>::max()))
    { // Programmer's error!
      // Can't be so few or so many points
      assert(0);
      return bb::mesh_t();
    }

    points.reserve(linePoints.size()*2);
    distance.reserve(linePoints.size()*2);
    indecies.reserve(linePoints.size()*3);

    uint16_t index = 0;
    for (auto item = linePoints.begin(), end = linePoints.end(); std::next(item) != end; ++item)
    {
      auto nextItem = std::next(item);

      auto dir = glm::normalize(*nextItem - *item);
      auto tangent = glm::vec2(dir.y, -dir.x)*width/2.0f;

      points.push_back(*item + tangent);
      points.push_back(*item - tangent);
      points.push_back(*nextItem + tangent);
      points.push_back(*nextItem - tangent);

      distance.emplace_back(0.0f);
      distance.emplace_back(1.0f);
      distance.emplace_back(0.0f);
      distance.emplace_back(1.0f);

      indecies.push_back(index++);
      indecies.push_back(index++);
      indecies.push_back(index++);
      indecies.push_back(index++);
      indecies.push_back(0xFFFF);
    }

    auto arrayBuffer = bb::vbo_t::CreateArrayBuffer(points, false);
    auto distBuffer = bb::vbo_t::CreateArrayBuffer(distance, false);
    auto elementsBuffer = bb::vbo_t::CreateElementArrayBuffer(indecies, false);
    auto line = bb::vao_t::CreateVertexAttribObject();

    line.BindVBO(arrayBuffer, 0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    line.BindVBO(distBuffer, 1, 1, GL_FLOAT, GL_FALSE, 0, 0);
    line.BindIndecies(elementsBuffer);

    return bb::mesh_t(std::move(line), indecies.size(), GL_TRIANGLE_STRIP, 2);
  }
  
}