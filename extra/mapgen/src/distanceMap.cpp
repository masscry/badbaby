#include <distanceMap.hpp>
#include <common.hpp>
#include <context.hpp>
#include <mailbox.hpp>

#include <set>

namespace 
{

  static const glm::ivec3 deltaVecs[] = {
    { -1, -1, -1 },
    {  0, -1, -1 },
    {  1, -1, -1 },
    { -1,  0, -1 },
    {  0,  0, -1 },
    {  1,  0, -1 },
    { -1,  1, -1 },
    {  0,  1, -1 },
    {  1,  1, -1 },
    { -1, -1,  0 },
    {  0, -1,  0 },
    {  1, -1,  0 },
    { -1,  0,  0 },
    {  1,  0,  0 },
    { -1,  1,  0 },
    {  0,  1,  0 },
    {  1,  1,  0 },
    { -1, -1,  1 },
    {  0, -1,  1 },
    {  1, -1,  1 },
    { -1,  0,  1 },
    {  0,  0,  1 },
    {  1,  0,  1 },
    { -1,  1,  1 },
    {  0,  1,  1 },
    {  1,  1,  1 }
  };

} // namespace

namespace bb
{
  namespace ext
  {

    template<typename type_t>
    bool Border(type_t value, type_t minVal, type_t maxVal)
    {
      return ((value >= minVal) && (value <= maxVal));
    }

    template<>
    bool Border<glm::ivec3>(glm::ivec3 value, glm::ivec3 minVal, glm::ivec3 maxVal)
    {
      return Border(value.x, minVal.x, maxVal.x)
        && Border(value.y, minVal.y, maxVal.y)
        && Border(value.z, minVal.z, maxVal.z);
    }

    template<>
    bool Border<glm::vec3>(glm::vec3 value, glm::vec3 minVal, glm::vec3 maxVal)
    {
      return Border(value.x, minVal.x, maxVal.x)
        && Border(value.y, minVal.y, maxVal.y)
        && Border(value.z, minVal.z, maxVal.z);
    }

    distanceMap_t::distanceMap_t()
    : width(0),
      height(0),
      depth(0)
    {

    }

    distanceMap_t::distanceMap_t(const heightMap_t& hmapSrc, size_t depth)
    : hmap(hmapSrc),
      width(hmap.Width()),
      height(hmap.Height()),
      depth(depth & 0xFFFF)
    {
      assert(this->width * this->height * this->depth != 0);
      if (this->width * this->height * this->depth != 0)
      {
        this->data.reset(
          new float[this->width*this->height*this->depth]
        );
        std::fill(
          this->data.get(),
          this->data.get() + this->width*this->height*this->depth,
          std::numeric_limits<float>::max()
        );
      }

      auto hmapMin = hmap.Min();
      auto hmapMax = hmap.Max();

      auto hmapStep = (hmapMax - hmapMin)/(depth-1);

      bb::Debug("Height Map Step: %f", hmapStep);

      using ivecPair_t = std::pair<glm::ivec3, float>;

      struct compareIvecPair_t
      {
        bool operator()(const ivecPair_t& a, const ivecPair_t& b) const
        {
          if (a.first == b.first)
          {
            return false;
          }
          return a.second < b.second;
        }
      };

      std::set<std::pair<glm::ivec3, float>, compareIvecPair_t> mapOfDistance;

      for (size_t y = 0; y < this->Height(); ++y)
      {
        for (size_t x = 0; x < this->Width(); ++x)
        {
          auto height = this->hmap.Data(x, y);
          for (size_t z = 0; z < this->Depth(); ++z)
          {
            auto hbias = fabsf(z*hmapStep - height);

            this->Data(x, y, z) = hbias;
            mapOfDistance.emplace(
              std::make_pair(glm::ivec3(x, y, z), hbias)
            );
          }
        }
      }

      bb::Debug("Initial Size: %lu", mapOfDistance.size());

      const auto minBorder = glm::ivec3(0, 0, 0);
      const auto maxBorder = glm::ivec3(this->Width(), this->Height(), this->Depth());

      size_t rounds = 0;
      while (!mapOfDistance.empty())
      {
        if (rounds++ % 1000 == 0)
        {
          if (bb::context_t::IsAlreadyExists())
          {
            auto& context = bb::context_t::Instance();
            context.Title(std::to_string(rounds) + "/" + std::to_string(mapOfDistance.size()));
          }
          bb::Debug("%lu", mapOfDistance.size());
        }

        auto coords = *mapOfDistance.begin();
        mapOfDistance.erase(mapOfDistance.begin());

        for (auto delta: deltaVecs)
        {
          auto side = coords.first + delta;
          if (Border(side, minBorder, maxBorder))
          {
            float minDist = coords.second + glm::length(glm::vec3(delta));
            if (this->Data(side) >= minDist)
            {
              auto newItem = std::make_pair(side, minDist);
              auto foundItem = mapOfDistance.find(newItem);

              if (foundItem == mapOfDistance.end())
              {
                mapOfDistance.emplace(newItem);
              }
              else
              {
                if (foundItem->second > newItem.second)
                {
                  mapOfDistance.erase(foundItem);
                  mapOfDistance.emplace(newItem);
                }
              }
              this->Data(side) = minDist;
            }
          }
        }
      }
    }

    distanceMap_t::distanceMap_t(glm::ivec3 dim)
    : width(dim.x & 0xFFFF),
      height(dim.y & 0xFFFF),
      depth(dim.z & 0xFFFF)
    {
      assert(this->width * this->height * this->depth != 0);
      if (this->width * this->height * this->depth != 0)
      {
        this->data.reset(
          new float[this->width*this->height*this->depth]
        );
        std::fill(
          this->data.get(),
          this->data.get() + this->width*this->height*this->depth,
          0.0f
        );
      }
    }

    distanceMap_t::distanceMap_t(const distanceMap_t& src)
    : hmap(src.hmap),
      width(src.width),
      height(src.height),
      depth(src.depth)
    {
      if (src.IsGood())
      {
        this->data.reset(
          new float[this->width*this->height*this->depth]
        );
        std::copy(
          src.data.get(), src.data.get() + this->width*this->height*this->depth,
          this->data.get()
        );
      }
    }

    distanceMap_t& distanceMap_t::operator=(const distanceMap_t& src)
    {
      if (this != &src)
      {
        if (src.IsGood())
        {
          this->hmap = src.hmap;
          this->width = src.width;
          this->height = src.height;
          this->depth = src.depth;
          this->data.reset(
            new float[this->width*this->height*this->depth]
          );
          std::copy(
            src.data.get(), src.data.get() + this->width*this->height*this->depth,
            this->data.get()
          );
        }
        else
        {
          this->hmap.Clear();
          this->width = 0;
          this->height = 0;
          this->depth = 0;
          this->data.reset();
        }
      }
      return *this;
    }

    float distanceMap_t::Sample(vec3_t pos) const
    {
      if (!this->IsGood())
      { // programmer's mistake
        assert(0);
        return 0.0f;
      }

      auto posInCell = modulo(pos, vec3_t(1.0f));
      auto tl = glm::uvec3(
        static_cast<unsigned int>(modulo(pos.x, this->Width()-1.0f)),
        static_cast<unsigned int>(modulo(pos.y, this->Height()-1.0f)),
        static_cast<unsigned int>(modulo(pos.z, this->Depth()-1.0f))
      );

      auto br = glm::uvec3(
        static_cast<unsigned int>(modulo(pos.x+1.0f, this->Width()-1.0f)),
        static_cast<unsigned int>(modulo(pos.y+1.0f, this->Height()-1.0f)),
        static_cast<unsigned int>(modulo(pos.z+1.0f, this->Depth()-1.0f))
      );

      float c[2][2][2] = {
        {
          { this->Data(tl.x, tl.y, tl.z), this->Data(tl.x, tl.y, br.z) }, 
          { this->Data(br.x, tl.y, tl.z), this->Data(br.x, tl.y, br.z) } 
        },
        { 
          { this->Data(tl.x, br.y, tl.z), this->Data(tl.x, br.y, br.z) }, 
          { this->Data(br.x, br.y, tl.z), this->Data(br.x, br.y, br.z) } 
        }
      };

      float cX[2][2] = {
        { glm::mix(c[0][0][0], c[1][0][0], posInCell.x), glm::mix(c[0][0][1], c[1][0][1], posInCell.x) },
        { glm::mix(c[0][1][0], c[1][1][0], posInCell.x), glm::mix(c[0][1][1], c[1][1][1], posInCell.x) }
      };

      float cXY[2] = {
        glm::mix(cX[0][0], cX[1][0], posInCell.y),
        glm::mix(cX[0][1], cX[1][1], posInCell.y)
      };

      return glm::mix(cXY[0], cXY[1], posInCell.z);
    }

    float distanceMap_t::MaxValue() const
    {
      if (!this->IsGood())
      {
        return std::numeric_limits<float>::lowest();
      }

      auto total = this->DataSize();
      auto maxValue = std::numeric_limits<float>::lowest();

      for (float* cursor = this->data.get(); total-->0; ++cursor)
      {
        maxValue = (*cursor > maxValue)?(*cursor):(maxValue);
      }

      return maxValue;
    }

    float distanceMap_t::MinValue() const
    {
      if (!this->IsGood())
      {
        return std::numeric_limits<float>::max();
      }

      auto total = this->DataSize();
      auto minValue = std::numeric_limits<float>::max();

      for (float* cursor = this->data.get(); total-->0; ++cursor)
      {
        minValue = (*cursor < minValue)?(*cursor):(minValue);
      }

      return minValue;
    }

    int distanceMap_t::Dump(const std::string& fname) const
    {
      auto maxValue = this->MaxValue();

      for (size_t z = 0; z < this->Depth(); ++z)
      {
        FILE* output = fopen((fname + std::to_string(z) + ".pgm") .c_str(), "wb");
        if (output == nullptr)
        {
          return -1;
        }
        BB_DEFER(fclose(output));

        fprintf(output, "P6\n%u %u\n%u\n", this->Width(), this->Height(), 255);

        for (size_t y = 0; y < this->Height(); ++y)
        {
          for (size_t x = 0; x < this->Width(); ++x)
          {
            auto data = this->Data(x, y, z);

            if (data > 0.0f)
            {
              data /= maxValue;
              data *= 255;
            }

            struct {
              uint8_t r;
              uint8_t g;
              uint8_t b;
            } pixel;

            if (data < 0.0f)
            {
              pixel.r = 0xFF;
              pixel.g = 0x60;
              pixel.b = 0x62;
            }
            else
            {
              pixel.r = pixel.g = pixel.b = static_cast<uint8_t>(data);
            }
            fwrite(&pixel, 3, 1, output);
          }
        }
      }
      return 0;
    }

    float distanceMap_t::SampleHeightMap(vec3_t pos) const
    {
      if (this->hmap.IsGood())
      {
        return pos.z - this->hmap.Sample(pos.x, pos.y)*(this->Depth()-1);
      }
      else
      {
        assert(0);
        return 0.0f;
      }
    }

    bool distanceMap_t::Improve(vec3_t start, vec3_t finish, vec3_t* isec) const
    {
      auto startSample = start.z - this->SampleHeightMap(start);
      auto finishSample = finish.z - this->SampleHeightMap(finish);
      size_t rounds = 5;

      while ((startSample > 0.0f) && (finishSample <= 0.0f))
      {
        auto center = (start + finish)/2.0f;
        auto centerSample = this->SampleHeightMap(center);

        --rounds;
        if (rounds == 0)
        {
          if (isec != nullptr)
          {
            *isec = center;
          }
          return true;
        }
        if (centerSample > 0.0f)
        {
          start = center;
          startSample = centerSample;
          continue;
        }
        if (centerSample <= 0.0f)
        {
          finish = center;
          finishSample = centerSample;
          continue;
        }
        if (isec != nullptr)
        {
          *isec = center;
        }
        return true;
      }

      if (isec != nullptr)
      {
        *isec = finish;
      }
      return true;
    }

    bool distanceMap_t::CastRay(vec3_t pos, vec3_t dir, vec3_t* isec, float maxDist) const
    {
      if (!Border(pos.z, 0.0f, static_cast<float>(this->Depth())))
      {
        return false;
      }

      if (glm::abs(glm::length(dir) - 1.0f) >= 0.1f)
      {
        assert(0);
        return false;
      }

      float hereSample;
      if (this->hmap.IsGood())
      {
        hereSample = this->SampleHeightMap(pos);
      }
      else
      {
        hereSample = this->Sample(pos);
      }
      if (hereSample < 0.0f)
      {
        if (isec != nullptr)
        {
          *isec = pos;
        }
        return true;
      }

      if (maxDist < 0.0f)
      {
        maxDist = INFINITY;
      }

      vec3_t prevCursor;
      vec3_t cursor = pos;
      while(glm::length(cursor - pos) < maxDist)
      {
        auto moveDist = fabsf(this->Sample(cursor));

        if (moveDist < 0.01f)
        {
          moveDist = 0.01f;
        }

        prevCursor = cursor;
        cursor += dir*moveDist;

        if (!Border(cursor.z, 0.0f, this->Dimensions().z))
        {
          return false;
        }

        if (this->hmap.IsGood())
        {
          hereSample = this->SampleHeightMap(cursor);
        }
        else
        {
          hereSample = this->Sample(cursor);
        }

        if (hereSample < 0.0f)
        {
          if (this->hmap.IsGood())
          {
            return this->Improve(prevCursor, cursor, isec);
          }
          else
          {
            if (isec != nullptr)
            {
              *isec = cursor;
            }
            return true;
          }
        }
      }
      return false;
    }

    namespace
    {
      struct distanceMapHeader_t
      {
        uint16_t width;
        uint16_t height;
        uint16_t depth;
        uint16_t hasHeightMap;
      };

    }

    int distanceMap_t::Serialize(binstore_t& output)
    {
      if ((!output.IsGood()) || (!this->IsGood()))
      {
        return -1;
      }

      distanceMapHeader_t head;
      head.width = this->Width();
      head.height = this->Height();
      head.depth = this->Depth();
      head.hasHeightMap = this->hmap.IsGood();

      if (output.Write(head) != 0)
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
      if (this->hmap.IsGood())
      {
        return this->hmap.Serialize(output);
      }
      return 0;
    }

    distanceMap_t::distanceMap_t(binstore_t& input)
    : width(0),
      height(0),
      depth(0)
    {
      distanceMapHeader_t head;
      if (input.IsGood() && (input.Read(head) == 0))
      {
        this->width = head.width;
        this->height = head.height;
        this->depth = head.depth;
        this->data.reset(new float[this->width*this->height*this->depth]);

        for (size_t index = 0; index < this->DataSize(); ++index)
        {
          if (input.Read(this->data[index]) != 0)
          {
            throw std::runtime_error("Invalid distance map format!");
          }
        }
        if (head.hasHeightMap != false)
        {
          this->hmap = heightMap_t(input);
        }
      }
    }

  } // namespace ext
} // namespace bb
