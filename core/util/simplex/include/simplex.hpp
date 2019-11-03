/**
 * @file simplex.hpp
 * 
 * Simple Open Simplex implementation
 * 
 */

#pragma once
#ifndef __BB_UTIL_SIMPLEX_HEADER__
#define __BB_UTIL_SIMPLEX_HEADER__

#include <cstdint>
#include <memory>

#include <glm/vec3.hpp>

namespace bb
{

  class simplex_t final
  {
    std::unique_ptr<uint8_t[]> perm;   // permutation array
    std::unique_ptr<uint8_t[]> perm3D; // permutation array in 3D
  public:

    simplex_t(long seed);

    simplex_t(const simplex_t& cp);
    simplex_t(simplex_t&& mv);

    simplex_t& operator=(const simplex_t& cp);
    simplex_t& operator=(simplex_t&& mv);

    ~simplex_t() = default;

    double operator()(double x, double y, double z) const;

    double operator()(glm::dvec3 v) const
    {
      return this->operator()(v.x, v.y, v.z);
    }

  };

} // namespace bb

#endif /* __BB_UTIL_SIMPLEX_HEADER__ */