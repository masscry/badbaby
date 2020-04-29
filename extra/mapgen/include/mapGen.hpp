/**
 * @file mapGen.hpp
 * 
 * Map Generator actor
 */
#pragma once
#ifndef __SUB3000_MAP_GEN_HEADER__
#define __SUB3000_MAP_GEN_HEADER__

#include <actor.hpp>
#include <role.hpp>
#include <camera.hpp>

#include <memory>
#include <random>

#include <heightMap.hpp>
#include <distanceMap.hpp>

namespace bb
{

  namespace ext
  {

    class generate_t: public msg::basic_t
    {
    public:
      int64_t seed;
      size_t width;
      size_t height;
      float radiusStart;
      float radiusFinish;
      size_t radiusRounds;
      float falloff;
      float power;

      vec2_t Dimension() const
      {
        return vec2_t(
          static_cast<float>(this->width),
          static_cast<float>(this->height)
        );
      }

      generate_t(actorPID_t src, size_t width, size_t height, float start, float finish, int64_t seed, float falloff, size_t rounds, float power)
      : msg::basic_t(src),
        seed(seed),
        width(width),
        height(height),
        radiusStart(start),
        radiusFinish(finish),
        radiusRounds(rounds),
        falloff(falloff),
        power(power)
      {
        ;
      }

      generate_t(const generate_t&) = default;
      generate_t& operator=(const generate_t&) = default;
      generate_t(generate_t&&) = default;
      generate_t& operator=(generate_t&&) = default;
      ~generate_t() override = default;
    };

    class hmDone_t: public msg::basic_t
    {
      heightMap_t heightMap;
      distanceMap_t distMap;
    public:

      distanceMap_t& DistanceMap()
      {
        return this->distMap;
      }

      heightMap_t& HeightMap()
      {
        return this->heightMap;
      }

      const distanceMap_t& DistanceMap() const
      {
        return this->distMap;
      }

      const heightMap_t& HeightMap() const
      {
        return this->heightMap;
      }

      hmDone_t(actorPID_t src, heightMap_t&& heightMap, distanceMap_t&& distMap)
      : msg::basic_t(src),
        heightMap(std::move(heightMap)),
        distMap(std::move(distMap))
      {
        ;
      }

      hmDone_t(const hmDone_t&) = default;
      hmDone_t& operator=(const hmDone_t&) = default;
      hmDone_t(hmDone_t&&) = default;
      hmDone_t& operator=(hmDone_t&&) = default;

      ~hmDone_t() override = default;

    };

    class mapGen_t final: public role_t
    {
      msg::result_t OnProcessMessage(const actor_t&, const msg::basic_t& msg) override;
    public:

      const char* DefaultName() const override
      {
        return "mapGenSV";
      }

      mapGen_t();
      ~mapGen_t() override;

    };

  } // namespace ext

}

#endif /* __SUB3000_MAP_GEN_HEADER__ */