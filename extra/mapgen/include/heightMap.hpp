/**
 * @file heightMap.hpp
 * 
 * Height map routines
 * 
 */

#pragma once
#ifndef __BB_EXTRA_HEIGHT_MAP_HEADER__
#define __BB_EXTRA_HEIGHT_MAP_HEADER__

#include <cstdint>
#include <memory>

#include <algebra.hpp>

namespace bb
{
  namespace ext
  {
    class heightMap_t final
    {
      std::unique_ptr<float[]> data;
      uint16_t width;
      uint16_t height;

    public:

      float Data(vec2_t pos) const;
      float DxDy(vec2_t pos, vec2_t dir) const;

      uint16_t Width() const;
      uint16_t Height() const;

      float* Data();
      const float* Data() const;

      float& Data(size_t x, size_t y);
      float Data(size_t x, size_t y) const;

      bool RayCast(vec2_t pos, vec2_t dir, float height, float step, float dist, vec2_t* pISec);

      float Max() const;
      float Min() const;

      heightMap_t& operator += (float val);
      heightMap_t& operator -= (float val);
      heightMap_t& operator *= (float val);
      heightMap_t& operator /= (float val);

      bool IsGood() const;

      heightMap_t();
      heightMap_t(uint16_t width, uint16_t height);

      heightMap_t(const heightMap_t& src);
      heightMap_t& operator=(const heightMap_t& src);

      heightMap_t(heightMap_t&&) = default;
      heightMap_t& operator=(heightMap_t&&) = default;
    };

    inline uint16_t heightMap_t::Width() const
    {
      return this->width;
    }

    inline uint16_t heightMap_t::Height() const
    {
      return this->height;
    }

    inline float* heightMap_t::Data()
    {
      return this->data.get();
    }

    inline const float* heightMap_t::Data() const
    {
      return this->data.get();
    }

    inline float& heightMap_t::Data(size_t x, size_t y)
    {
      return this->data[y * this->width + x];
    }

    inline float heightMap_t::Data(size_t x, size_t y) const
    {
      return this->data[y * this->width + x];
    }

    inline bool heightMap_t::IsGood() const
    {
      return ((this->width * this->height) && (this->data));
    }

  } // namespace ext

} // namespace bb


#endif /* __BB_EXTRA_HEIGHT_MAP_HEADER__ */