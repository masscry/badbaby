/**
 * @file image.hpp
 * 
 * Simple image class
 *
 */

#pragma once
#ifndef __BB_CORE_COMMON_TARGA_HEADER__
#define __BB_CORE_COMMON_TARGA_HEADER__

#include <cstdint>
#include <memory>

namespace bb
{

  class image_t final
  {
    std::unique_ptr<uint8_t[]> data;
    int width;
    int height;
    int depth;

  public:

    const uint8_t* Data() const 
    {
      return data.get();
    }

    int Width() const
    {
      return this->width;
    }

    int Height() const
    {
      return this->height;
    }

    int Depth() const
    {
      return this->depth;
    }

    image_t(const image_t& copy);
    image_t(image_t&& copy);

    image_t& operator=(const image_t& copy);
    image_t& operator=(image_t&& copy);

    image_t();
    image_t(std::unique_ptr<uint8_t[]> data, int width, int height, int depth);
    ~image_t() = default;
  };

  image_t LoadTGA(const char* filename);

}

#endif /* __BB_CORE_COMMON_TARGA_HEADER__ */