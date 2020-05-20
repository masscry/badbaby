/**
 * @file distanceMap.hpp
 * 
 * Distance 3D map routines.
 * 
 */

#pragma once
#ifndef __BB_EXTRA_DISTANCE_MAP_HEADER__
#define __BB_EXTRA_DISTANCE_MAP_HEADER__

#include <heightMap.hpp>
#include <binstore.hpp>

namespace bb
{
  namespace ext
  {

    class distanceMap_t final
    {
      heightMap_t hmap;
      std::unique_ptr<float[]> data;
      uint16_t width;
      uint16_t height;
      uint16_t depth;

      bool Improve(vec3_t start, vec3_t finish, vec3_t* isec) const;

      float SampleHeightMap(vec3_t pos) const;

    public:

      const heightMap_t& HeightMap() const;

      bb::vec3_t Dimensions() const;

      float MaxValue() const;

      float MinValue() const;

      bool CastRay(vec3_t pos, vec3_t dir, vec3_t* isec, float maxDist) const;

      float Sample(vec3_t pos) const;

      template<typename... args_t>
      float Sample(args_t&&... args) const
      {
        return this->Sample(vec3_t(std::forward<args_t>(args)...));
      }

      float& Data(size_t x, size_t y, size_t z);

      float Data(size_t x, size_t y, size_t z) const;

      float& Data(const glm::ivec3& v);

      float Data(const glm::ivec3& v) const;

      size_t DataSize() const;

      uint16_t Width() const;
      uint16_t Height() const;
      uint16_t Depth() const;

      bool IsGood() const;

      float operator[](const vec3_t& pos) const;

      int Dump(const std::string& fname) const;

      int Serialize(binstore_t& output);

      distanceMap_t(binstore_t& input);

      distanceMap_t();
      distanceMap_t(glm::ivec3 dim);
      distanceMap_t(const heightMap_t& hmap, size_t depth);

      distanceMap_t(const distanceMap_t& src);
      distanceMap_t& operator=(const distanceMap_t& src);

      distanceMap_t(distanceMap_t&&) = default;
      distanceMap_t& operator=(distanceMap_t&&) = default;
    };

    inline const heightMap_t& distanceMap_t::HeightMap() const
    {
      return this->hmap;
    }

    inline size_t distanceMap_t::DataSize() const
    {
      return this->Width() * this->Height() * this->Depth();
    }

    inline uint16_t distanceMap_t::Width() const
    {
      return this->width; 
    }

    inline uint16_t distanceMap_t::Height() const
    {
      return this->height;
    }

    inline uint16_t distanceMap_t::Depth() const
    {
      return this->depth;
    }

    inline bb::vec3_t distanceMap_t::Dimensions() const
    {
      return bb::vec3_t(
        static_cast<float>(this->Width()),
        static_cast<float>(this->Height()),
        static_cast<float>(this->Depth())
      );
    }

    inline bool distanceMap_t::IsGood() const
    {
      return static_cast<bool>(this->data);
    }

    inline float& distanceMap_t::Data(size_t x, size_t y, size_t z)
    {
      x = glm::clamp<size_t>(x, 0, this->Width()-1);
      y = glm::clamp<size_t>(y, 0, this->Height()-1);
      z = glm::clamp<size_t>(z, 0, this->Depth()-1);
      return this->data[static_cast<size_t>(x + this->Width() * (y + this->Height() * z))];
    }

    inline float distanceMap_t::Data(size_t x, size_t y, size_t z) const
    {
      return (const_cast<distanceMap_t*>(this))->Data(x, y, z);
    }

    inline float& distanceMap_t::Data(const glm::ivec3& v)
    {
      return this->Data(
        static_cast<size_t>(v.x),
        static_cast<size_t>(v.y),
        static_cast<size_t>(v.z)
      );
    }

    inline float distanceMap_t::Data(const glm::ivec3& v) const
    {
      return (const_cast<distanceMap_t*>(this))->Data(v);
    }

  } // namespace ext
} // namespace bb

#endif /* __BB_EXTRA_DISTANCE_MAP_HEADER__ */