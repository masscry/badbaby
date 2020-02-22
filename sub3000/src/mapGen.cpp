#include <mapGen.hpp>
#include <simplex.hpp>
#include <worker.hpp>

#include <cassert>
#include <cmath>

#include <glm/gtc/constants.hpp>
#include <glm/vec3.hpp>

namespace sub3000
{

  bb::msg::result_t mapGen_t::OnProcessMessage(const bb::actor_t& actor, const bb::msg::basic_t& msg)
  {

    if (auto genParams = bb::msg::As<sub3000::generate_t>(msg))
    {
      auto simplex = bb::simplex_t(dist(mt));

      heightMap_t heightMap;
      if ((genParams->Width() * genParams->Height() == 0) || (genParams->Radius() == 0.0f))
      {
        throw std::runtime_error("One of map dimensions equals zero!");
      }

      heightMap.width = genParams->Width();
      heightMap.height = genParams->Height();
      heightMap.data.reset(new float[heightMap.width*heightMap.height]);

      float radius = genParams->Radius();

      for (int row = 0; row < heightMap.height; ++row)
      {
        double rowZ = row/static_cast<double>(heightMap.height)*10.0;
        for (int col = 0; col < heightMap.width; ++col)
        {
          double angle = (col/static_cast<double>(heightMap.width))*glm::two_pi<double>();
          heightMap.data[static_cast<size_t>(row*heightMap.width + col)] = static_cast<float>(
            1.0 - simplex(glm::dvec3(cos(angle)*radius, sin(angle)*radius, rowZ))
          );
        }
      }
      bb::workerPool_t::Instance().PostMessage(
        genParams->Source(),
        bb::Issue<done_t>(actor.ID(), std::move(heightMap))
      );
      return bb::msg::result_t::complete;
    }

    bb::Error("Unknown message: %s", typeid(msg).name());
    assert(0);
    return bb::msg::result_t::error;
  }

  mapGen_t::mapGen_t()
  : rd(),
    mt(rd()),
    dist()
  {
    ;
  }

  mapGen_t::~mapGen_t()
  {
    ;
  }


} // sub3000
