#include <common.hpp>
#include <mapGen.hpp>
#include <simplex.hpp>
#include <worker.hpp>

#include <cassert>
#include <cmath>

#include <glm/gtc/constants.hpp>
#include <glm/vec3.hpp>

#ifdef _WIN32

void sincos(double x, double *sinVal, double *cosVal)
{
  BB_ASSERT((sinVal != nullptr) && (cosVal != nullptr));
  *sinVal = sin(x);
  *cosVal = cos(x);
}

#endif

namespace bb
{

  namespace ext
  {

    heightMap_t MakeHMapUsingOctaves(const generate_t& params)
    {
      BB_ASSERT((params.width < 0x10000) && (params.height < 0x10000));
      auto simplex = simplex_t(params.seed);

      heightMap_t heightMap(params.width & 0xFFFF, params.height & 0xFFFF);

      double radiusStart = params.radiusStart;
      double radiusFinish = params.radiusFinish;
      auto maxRadiusRounds = params.radiusRounds;

      std::vector<double> octave(maxRadiusRounds);

      auto resolution = params.Dimension();

      size_t cursor = 0;

      for(size_t y = 0; y < params.height; ++y)
      {
        double sinTheta;
        double cosTheta;
        sincos(
          static_cast<double>(y)/resolution.y*M_PI,
          &sinTheta,
          &cosTheta
        );

        for (size_t x = 0; x < params.width; ++x)
        {
          double phi = -M_PI + static_cast<double>(y)/resolution.x*M_PI*2.0;
          double sinPhi;
          double cosPhi;
          sincos(phi, &sinPhi, &cosPhi);

          glm::dvec3 coords(sinTheta*cosPhi, sinTheta*sinPhi, cosTheta);
          for (auto round = 0u; round < maxRadiusRounds; ++round)
          {
            auto radius = glm::mix(radiusStart, radiusFinish, round / static_cast<double>(maxRadiusRounds));
            octave[round] = simplex(coords * radius) * pow(params.falloff, round);
            octave[round] = pow(fabs(octave[round]), params.power) * bb::signum(octave[round]);
          }

          heightMap[cursor] = 0.0f;
          for (auto octaveVal : octave)
          {
            heightMap[cursor] += static_cast<float>(octaveVal);
          }
          ++cursor;
        }
      }

      float minPixel = heightMap.Min();
      float maxPixel = heightMap.Max();

      float lenPixel = maxPixel - minPixel;

      heightMap -= minPixel;
      heightMap /= lenPixel;

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
        auto distMap = bb::ext::distanceMap_t(heightMap, 64);

        workerPool_t::Instance().PostMessage(
            genParams->Source(),
            Issue<hmDone_t>(actor.ID(), std::move(heightMap), std::move(distMap)));
        return msg::result_t::complete;
      }

      Error("Unknown message: %s", typeid(msg).name());
      BB_PANIC();
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
