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
#include <utility>

#include <algebra.hpp>
#include <binstore.hpp>

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

      void Clear();

      float& operator[](size_t pos);
      float operator[](size_t pos) const;

      float Sample(vec2_t pos) const;

      template<typename... args_t>
      float Sample(args_t&&... args) const
      {
        return this->Sample(vec2_t(std::forward<args_t>(args)...));
      }

      float Sample(vec3_t pos) const;

      size_t DataSize() const;

      uint16_t Width() const;
      uint16_t Height() const;

      int Dump(const std::string& fname) const;

      float* Data();
      const float* Data() const;

      float& Data(size_t x, size_t y);
      float Data(size_t x, size_t y) const;

      float Max() const;
      float Min() const;

      heightMap_t& operator += (float val);
      heightMap_t& operator -= (float val);
      heightMap_t& operator *= (float val);
      heightMap_t& operator /= (float val);

      bool IsGood() const;

      int Serialize(bb::ext::binstore_t& output);

      heightMap_t(bb::ext::binstore_t& input);

      heightMap_t();
      heightMap_t(uint16_t width, uint16_t height);

      heightMap_t(const heightMap_t& src);
      heightMap_t& operator=(const heightMap_t& src);

      heightMap_t(heightMap_t&&) = default;
      heightMap_t& operator=(heightMap_t&&) = default;
    };

    inline float heightMap_t::Sample(vec3_t pos) const
    {
      float result = this->Sample(vec2_t(pos));
      return pos.z - result;
    }

    inline float& heightMap_t::operator[](size_t pos)
    {
      return this->data[pos];
    }

    inline float heightMap_t::operator[](size_t pos) const
    {
      return this->data[pos];
    }

    inline size_t heightMap_t::DataSize() const
    {
      return this->Width() * this->Height();
    }

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
      return (const_cast<heightMap_t*>(this))->Data(x, y);
    }

    inline bool heightMap_t::IsGood() const
    {
      return ((this->width * this->height) && (this->data));
    }

  } // namespace ext

} // namespace bb


#endif /* __BB_EXTRA_HEIGHT_MAP_HEADER__ */