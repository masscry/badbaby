#include <heightMap.hpp>

#include <cassert>
#include <cstring>
#include <cmath>

#include <limits>

#include <common.hpp>

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

    float heightMap_t::Sample(vec2_t pos) const
    {
      if (!this->IsGood())
      { // programmer's mistake
        assert(0);
        return 0.0f;
      }

      auto posInCell = modulo(pos, vec2_t(1.0f));
      auto topLeftX = static_cast<size_t>(modulo(pos.x, this->Width()));
      auto topLeftY = static_cast<size_t>(modulo(pos.y, this->Height()));

      float hmatrix[2][2] = {
        { this->Data(topLeftX, topLeftY),   this->Data(topLeftX+1, topLeftY)   },
        { this->Data(topLeftX, topLeftY+1), this->Data(topLeftX+1, topLeftY+1) }
      };

      return hmatrix[0][0]*(1.0f-posInCell.x)*(1.0f-posInCell.y)
        + hmatrix[0][1]*posInCell.x*(1.0f-posInCell.y)
        + hmatrix[1][0]*(1.0f-posInCell.x)*posInCell.y
        + hmatrix[1][1]*posInCell.x*posInCell.y;
    }

    int heightMap_t::Dump(const std::string& fname) const
    {
      FILE* output = fopen((fname + ".pgm").c_str(), "w");
      if (output == nullptr)
      {
        return -1;
      }
      BB_DEFER(fclose(output));

      fprintf(output, "P2\n%u %u\n%u\n", this->Width(), this->Height(), 255);

      for (size_t y = 0; y < this->Height(); ++y)
      {
        for (size_t x = 0; x < this->Width(); ++x)
        {
          fprintf(output, "%u ", (static_cast<uint32_t>(this->Data(x, y)*0xFF) & 0xFF));
        }
        fprintf(output, "\n");
      }
      return 0;
    }

    void heightMap_t::Clear()
    {
      this->data.reset();
      this->width = 0;
      this->height = 0;
    }

    int heightMap_t::Serialize(bb::ext::binstore_t& output)
    {
      if ((!output.IsGood()) || (!this->IsGood()))
      {
        return -1;
      }

      if (output.Write(this->width) != 0)
      {
        return -1;
      }
      if (output.Write(this->height) != 0)
      {
        return -1;
      }

      for (size_t index = 0; index < this->DataSize(); ++index)
      {
        if (output.Write(this->data[index]) != 0)
        {
          return -1;
        }
      }
      return 0;
    }

    heightMap_t::heightMap_t(bb::ext::binstore_t& input)
    : width(0),
      height(0)
    {
      if (input.IsGood()&&(input.Read(this->width) == 0)&&(input.Read(this->height)==0))
      {
        this->data.reset(new float[this->width*this->height]);
        for (size_t index = 0; index < this->DataSize(); ++index)
        {
          input.Read(this->data[index]);
        }
      }
    }

    bb::vec3_t heightMap_t::NormalAtPoint(bb::vec2_t pos, float zScale) const
    {
      auto p0 = glm::vec3(pos, this->Sample(pos)*zScale);

      auto p1 = glm::vec3(pos + glm::vec2(-1.0f,  0.0f), this->Sample(pos + glm::vec2(-1.0f,  0.0f))*zScale);
      auto p2 = glm::vec3(pos + glm::vec2( 0.0f, -1.0f), this->Sample(pos + glm::vec2( 0.0f, -1.0f))*zScale);
      auto p3 = glm::vec3(pos + glm::vec2( 1.0f,  0.0f), this->Sample(pos + glm::vec2( 1.0f,  0.0f))*zScale);
      auto p4 = glm::vec3(pos + glm::vec2( 0.0f,  1.0f), this->Sample(pos + glm::vec2( 0.0f,  1.0f))*zScale);

      auto v1 = p1 - p0;
      auto v2 = p2 - p0;
      auto v3 = p3 - p0;
      auto v4 = p4 - p0;

      auto v12 = glm::cross(v1, v2);
      auto v23 = glm::cross(v2, v3);
      auto v34 = glm::cross(v3, v4);
      auto v41 = glm::cross(v4, v1);

      return glm::normalize(
        v12 + v23 + v34 + v41
      );
    }

  } // namespace ext
  
} // namespace bb
