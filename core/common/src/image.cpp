#include <cassert>
#include <cstring>

#include <image.hpp>

namespace bb
{

  image_t::image_t(const image_t& copy)
  :data(nullptr),width(copy.width),height(copy.height),depth(copy.depth)
  {
    if (copy.data)
    {
      assert((copy.width != 0) && (copy.height != 0) && (copy.depth != 0));

      size_t dataSize = copy.width * copy.height * copy.depth;
      this->data.reset(new uint8_t[dataSize]);

      memcpy(this->data.get(), copy.data.get(), dataSize);
    }
  }

  image_t::image_t(image_t&& copy)
  :data(std::move(copy.data)),width(copy.width),height(copy.height),depth(copy.depth)
  {
    copy.width  = 0;
    copy.height = 0;
    copy.depth  = 0;
  }

  image_t& image_t::operator=(const image_t& copy)
  {
    if (this == &copy)
    {
      return *this;
    }
    if (copy.data)
    { // has data to copy
      assert((copy.width != 0) && (copy.height != 0) && (copy.depth != 0));

      size_t dataSize = copy.width * copy.height * copy.depth;
      this->data.reset(new uint8_t[dataSize]);

      memcpy(this->data.get(), copy.data.get(), dataSize);
      this->width  = copy.width;
      this->height = copy.height;
      this->depth  = copy.depth;
    }
    else
    { // empty image
      this->data.reset(nullptr);
      this->width  = 0;
      this->height = 0;
      this->depth  = 0;
    }
    return *this;
  }

  image_t& image_t::operator=(image_t&& copy)
  {
    if (this == &copy)
    {
      return *this;
    }
    this->data   = std::move(copy.data);
    this->width  = copy.width;
    this->height = copy.height; 
    this->depth  = copy.depth;

    copy.width  = 0;
    copy.height = 0;
    copy.depth  = 0;

    return *this;
  }

  image_t::image_t()
  :data(nullptr), width(0), height(0), depth(0)
  {
    ;
  }

  image_t::image_t(std::unique_ptr<uint8_t[]> data, int width, int height, int depth)
  :data(std::move(data)), width(width), height(height), depth(depth)
  {
    ;
  }

}