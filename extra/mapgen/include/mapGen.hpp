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

namespace bb
{

  namespace ext
  {

    class generate_t: public msg::basic_t
    {
    public:
      int64_t seed;
      uint16_t width;
      uint16_t height;
      float radiusStart;
      float radiusFinish;
      int radiusRounds;
      float falloff;
      float power;

      vec2_t Dimension() const
      {
        return vec2_t(
          static_cast<float>(this->width),
          static_cast<float>(this->height)
        );
      }

      generate_t(actorPID_t src, uint16_t width, uint16_t height, float start, float finish, int64_t seed, float falloff, int rounds, float power)
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

    struct heightMap_t final
    {
      std::unique_ptr<float[]> data;
      uint16_t width;
      uint16_t height;

      heightMap_t()
      : width(0),
        height(0)
      {
        ;
      }

      heightMap_t(const heightMap_t&) = delete;
      heightMap_t& operator=(const heightMap_t&) = delete;

      heightMap_t(heightMap_t&&) = default;
      heightMap_t& operator=(heightMap_t&&) = default;
    };

    class done_t: public msg::basic_t
    {
      heightMap_t heightMap;
    public:

      const heightMap_t& HeightMap() const
      {
        return this->heightMap;
      }

      done_t(actorPID_t src, heightMap_t&& heightMap)
      : msg::basic_t(src),
        heightMap(std::move(heightMap))
      {
        ;
      }

      done_t(const done_t&) = default;
      done_t& operator=(const done_t&) = default;
      done_t(done_t&&) = default;
      done_t& operator=(done_t&&) = default;

      ~done_t() override = default;

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