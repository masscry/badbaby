#include <heightMap.hpp>

#include <cassert>
#include <cstring>
#include <cmath>

#include <limits>

namespace bb
{

  namespace ext
  {

    float heightMap_t::Max() const
    {
      if (!this->IsGood())
      {
        assert(0);
        return -std::numeric_limits<float>::max();
      }

      float maxPixel = -std::numeric_limits<float>::max();
      for (size_t pixel = 0, total = this->height * this->width; pixel < total; ++pixel)
      {
        maxPixel = std::max(this->data[pixel], maxPixel);
      }
      return maxPixel;
    }

    float heightMap_t::Min() const
    {
      if (!this->IsGood())
      {
        assert(0);
        return std::numeric_limits<float>::max();
      }

      float minPixel = std::numeric_limits<float>::max();
      for (size_t pixel = 0, total = this->height * this->width; pixel < total; ++pixel)
      {
        minPixel = std::min(this->data[pixel], minPixel);
      }
      return minPixel;
    }

    heightMap_t& heightMap_t::operator += (float val)
    {
      for (size_t pixel = 0, total = this->height * this->width; pixel < total; ++pixel)
      {
        this->data[pixel] += val;
      }
      return *this;
    }

    heightMap_t& heightMap_t::operator -= (float val)
    {
      for (size_t pixel = 0, total = this->height * this->width; pixel < total; ++pixel)
      {
        this->data[pixel] -= val;
      }
      return *this;
    }

    heightMap_t& heightMap_t::operator *= (float val)
    {
      for (size_t pixel = 0, total = this->height * this->width; pixel < total; ++pixel)
      {
        this->data[pixel] *= val;
      }
      return *this;
    }

    heightMap_t& heightMap_t::operator /= (float val)
    {
      for (size_t pixel = 0, total = this->height * this->width; pixel < total; ++pixel)
      {
        this->data[pixel] /= val;
      }
      return *this;
    }
    
    heightMap_t::heightMap_t()
    : width(0),
      height(0)
    {
      ;
    }

    heightMap_t::heightMap_t(uint16_t width, uint16_t height)
    : width(width),
      height(height)
    {
      assert((this->width * this->height) != 0);
      this->data.reset(new float[this->width*this->height]);
    }

    heightMap_t::heightMap_t(const heightMap_t& src)
    : width(src.width),
      height(src.height)
    {
      if (src.IsGood())
      {
        this->data.reset(new float[this->width*this->height]);
        memcpy(this->data.get(), src.data.get(), this->width*this->height*sizeof(float));
      }
      else
      {
        this->width = 0;
        this->height = 0;
      }
    }

    heightMap_t& heightMap_t::operator=(const heightMap_t& src)
    {
      if (this != &src)
      {
        if (src.IsGood())
        {
          this->width = src.width;
          this->height = src.height;
          this->data.reset(new float[this->width*this->height]);
          memcpy(this->data.get(), src.data.get(), this->width*this->height*sizeof(float));
        }
        else
        {
          this->width = 0;
          this->height = 0;
          this->data.reset();
        }
      }
      return *this;
    }

    inline float modulo(float x, float y)
    {
      return fmodf(fmodf(x, y) + y, y);
    }

    bool heightMap_t::RayCast(vec2_t pos, vec2_t dir, float height, float dist, vec2_t* pISec)
    {
      if (!this->IsGood())
      { // programmer's mistake
        assert(0);
        return false;
      }

      auto posOnMapX = static_cast<size_t>(modulo(pos.x, this->Width()));
      auto posOnMapY = static_cast<size_t>(modulo(pos.y, this->Height()));

      float hereHeight = this->Data(posOnMapX, posOnMapY);
      if (hereHeight > height)
      {
        if (pISec != nullptr)
        {
          *pISec = pos;
        }
        return true;
      }

      vec2_t cursor = pos;
      float distHeight = hereHeight;
      do 
      {
        cursor += dir * dist;

        if (glm::length(cursor - pos) > 10.0f)
        {
          return false;
        }

        posOnMapX = static_cast<size_t>(modulo(cursor.x, this->Width()));
        posOnMapY = static_cast<size_t>(modulo(cursor.y, this->Height()));
        distHeight = this->Data(posOnMapX, posOnMapY);
      } while (distHeight <= height);

      if (pISec != nullptr)
      {
        *pISec = cursor;
      }
      return true;
    }


  } // namespace ext
  
} // namespace bb
