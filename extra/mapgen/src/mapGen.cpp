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
        auto simplex = simplex_t(genParams->Seed());

        heightMap_t heightMap;
        if ((genParams->Width() * genParams->Height() == 0) || (genParams->Radius() == 0.0f))
        {
          throw std::runtime_error("One of map dimensions equals zero!");
        }

        heightMap.width = genParams->Width();
        heightMap.height = genParams->Height();
        heightMap.data.reset(new float[heightMap.width*heightMap.height]);

        double radius = genParams->Radius();

        int maxRounds = genParams->Rounds();

        for (size_t row = 0; row < heightMap.height; ++row)
        {
          double theta = (row/static_cast<double>(heightMap.height))*M_PI;
          for (size_t col = 0; col < heightMap.width; ++col)
          {
            double phi = (col/static_cast<double>(heightMap.width))*M_PI*2.0f;
            glm::dvec3 coords(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta));
            heightMap.data[row * heightMap.width + col] = 0.0f;
            double falloff = 1.0;
            for (int round = 0; round < maxRounds; ++round)
            {
              double roundPart = (round+1)/static_cast<double>(maxRounds);
              heightMap.data[row * heightMap.width + col] += simplex(
                coords * radius*roundPart
              ) * falloff;
              falloff *= genParams->Falloff();
            }
          }
        }

        float minPixel = std::numeric_limits<float>::max();
        float maxPixel = -std::numeric_limits<float>::max();

        for (size_t pixel = 0; pixel < heightMap.height * heightMap.width; ++pixel)
        {
          minPixel = std::min(heightMap.data[pixel], minPixel);
          maxPixel = std::max(heightMap.data[pixel], maxPixel);
        }

        float lenPixel = maxPixel - minPixel;

        for (size_t pixel = 0; pixel < heightMap.height * heightMap.width; ++pixel)
        {
          heightMap.data[pixel] += minPixel;
        }

        for (size_t pixel = 0; pixel < heightMap.height * heightMap.width; ++pixel)
        {
          heightMap.data[pixel] /= lenPixel;
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
    {
      ;
    }

    mapGen_t::~mapGen_t()
    {
      ;
    }

  }

} // sub3000
