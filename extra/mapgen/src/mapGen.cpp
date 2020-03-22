#include <mapGen.hpp>
#include <simplex.hpp>
#include <worker.hpp>

#include <cassert>
#include <cmath>

#include <glm/gtc/constants.hpp>
#include <glm/vec3.hpp>

namespace bb
{

  namespace ext
  {

    msg::result_t mapGen_t::OnProcessMessage(const actor_t& actor, const msg::basic_t& msg)
    {

      if (auto genParams = msg::As<generate_t>(msg))
      {
        auto simplex = simplex_t(dist(mt));

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
        workerPool_t::Instance().PostMessage(
          genParams->Source(),
          Issue<done_t>(actor.ID(), std::move(heightMap))
        );
        return msg::result_t::complete;
      }

      Error("Unknown message: %s", typeid(msg).name());
      assert(0);
      return msg::result_t::error;
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

  }

} // sub3000
