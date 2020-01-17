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

#include <memory>
#include <random>

namespace sub3000
{

  enum class mapGenMsg_t: uint16_t
  {
    firstItem = bb::msgID_t::USR00,
    generate = firstItem,
    done,
    totalItems
  };

  struct heightMap_t
  {
    std::unique_ptr<float[]> data;
    uint16_t width;
    uint16_t height;
  };

  struct mapGenerateParams_t
  {
    uint16_t width;
    uint16_t height;
    float radius;
  };

  class mapGen_t final: public bb::role_t
  {
    std::random_device rd;
    std::mt19937 mt;
    std::uniform_int_distribution<long> dist;

    bb::msgResult_t OnProcessMessage(const bb::actor_t&, bb::msg_t msg) override;
  public:

    const char* DefaultName() const override
    {
      return "mapGenSV";
    }

    mapGen_t();
    ~mapGen_t() override;

  };

} // namespace sub3000

#endif /* __SUB3000_MAP_GEN_HEADER__ */