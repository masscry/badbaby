#include <mapGen.hpp>
#include <simplex.hpp>
#include <worker.hpp>

#include <cassert>
#include <cmath>

#include <glm/gtc/constants.hpp>
#include <glm/vec3.hpp>

namespace sub3000
{

  bb::msgResult_t mapGen_t::OnProcessMessage(const bb::actor_t& actor, bb::msg_t msg)
  {
    switch (static_cast<mapGenMsg_t>(msg.type))
    {
    case mapGenMsg_t::generate:
      {
        auto simplex = bb::simplex_t(dist(mt));

        std::unique_ptr<heightMap_t> heightMap(new heightMap_t);
        auto params = bb::GetMsgData<mapGenerateParams_t>(msg);
        if ((params.width * params.height == 0) || (params.radius == 0.0f))
        {
          throw std::runtime_error("One of map dimensions equals zero!");
        }
        heightMap->width = params.width;
        heightMap->height = params.height;
        heightMap->data.reset(new float[heightMap->width*heightMap->height]);

        for (int row = 0; row < heightMap->height; ++row)
        {
          double rowZ = row/static_cast<double>(heightMap->height)*10.0;
          for (int col = 0; col < heightMap->width; ++col)
          {
            double angle = (col/static_cast<double>(heightMap->width))*glm::two_pi<double>();
            heightMap->data[static_cast<size_t>(row*heightMap->width + col)] = static_cast<float>(
              1.0 - simplex(glm::dvec3(cos(angle)*params.radius, sin(angle)*params.radius, rowZ))
            );
          }
        }
        bb::workerPool_t::Instance().PostMessage(
          msg.src,
          bb::MakeMsgPtr(actor.ID(), mapGenMsg_t::done, heightMap.release())
        );
      }
      break;
    default:
      assert(0);
    }
    return bb::msgResult_t::complete;
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
