#include <mapGen.hpp>
#include <simplex.hpp>
#include <worker.hpp>
#include <common.hpp>

#include <cassert>
#include <cmath>

#include <glm/gtc/constants.hpp>
#include <glm/vec3.hpp>

namespace bb
{

  namespace ext
  {

    heightMap_t MakeHMapUsingOctaves(const generate_t& params)
    {
      auto simplex = simplex_t(params.seed);

      heightMap_t heightMap(params.width, params.height);

      double radiusStart = params.radiusStart;
      double radiusFinish = params.radiusFinish;
      auto maxRadiusRounds = params.radiusRounds;

      std::vector<double> octave(maxRadiusRounds);

      for (size_t row = 0; row < heightMap.height; ++row)
      {
        double theta = (row / static_cast<double>(heightMap.height)) * M_PI;
        for (size_t col = 0; col < heightMap.width; ++col)
        {
          double phi = (col / static_cast<double>(heightMap.width)) * M_PI * 2.0f;
          glm::dvec3 coords(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));

          for (auto round = 0u; round < maxRadiusRounds; ++round)
          {
            auto radius = glm::mix(radiusStart, radiusFinish, round / static_cast<double>(maxRadiusRounds));
            octave[round] = simplex(coords * radius) * pow(params.falloff, round);
            octave[round] = pow(fabs(octave[round]), params.power) * bb::signum(octave[round]);
          }

          for (auto octaveVal : octave)
          {
            heightMap.data[row * heightMap.width + col] += octaveVal;
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
        heightMap.data[pixel] -= minPixel;
        heightMap.data[pixel] /= lenPixel;
      }

      return heightMap;
    }

    msg::result_t mapGen_t::OnProcessMessage(const actor_t &actor, const msg::basic_t &msg)
    {

      if (auto genParams = msg::As<generate_t>(msg))
      {
        if ((genParams->width * genParams->height == 0) || (genParams->radiusStart == 0.0f) || (genParams->radiusFinish == 0.0f) || (genParams->radiusRounds == 0))
        {
          throw std::runtime_error("One of map dimensions equals zero!");
        }

        auto heightMap = MakeHMapUsingOctaves(*genParams);

        workerPool_t::Instance().PostMessage(
            genParams->Source(),
            Issue<done_t>(actor.ID(), std::move(heightMap)));
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

  } // namespace ext

} // namespace bb
