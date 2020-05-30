#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

#include <common.hpp>
#include <image.hpp>

namespace
{

  enum tgaErrors
  {
    TGA_OK = 0, /**< No errors */
    TGA_IO = 1, /**< IO error  */
    TGA_SIG = 2
  };

#pragma pack(push, 1)

  struct tgaColorMapSpec
  {
    uint16_t first;
    uint16_t len;
    uint8_t size;
  };

  struct tgaImageSpec
  {
    uint16_t xorig;
    uint16_t yorig;
    uint16_t width;
    uint16_t height;
    uint8_t depth;
    uint8_t alpha : 4; // 3-0
    uint8_t _zero : 1; // 4
    uint8_t orig : 1;  // 5
    uint8_t _res : 2;
  };

  struct tgaHeader
  {
    uint8_t idlen;
    uint8_t cmtype;
    uint8_t imtype;
    tgaColorMapSpec cms;
    tgaImageSpec is;
  };

  struct tgaFooter
  {
    uint32_t extoff;
    uint32_t devoff;
    char sig[18];
  };

#pragma pack(pop)

  const char TGA_SIGNATURE[] = "TRUEVISION-XFILE.";

  enum class tgaColorMode_t
  {
    unknown = 0,
    rgba,
    bw
  };

  tgaColorMode_t tgaGetColorMode(uint8_t imtype)
  {
    switch (imtype & 0x3)
    {
      case 0x2:
        return tgaColorMode_t::rgba;
      case 0x3:
        return tgaColorMode_t::bw;
      default:
        return tgaColorMode_t::unknown;
    }
  }

  bool tgaIsRLE(uint8_t imtype)
  {
    return (imtype & 0x8) == 0x8;
  }

  void tgaRLEReadRGBA(uint8_t *data, const tgaHeader head, FILE *input)
  {
    uint8_t *cursor = data;

    while (cursor - data < static_cast<off_t>(head.is.width) * head.is.height * 4)
    {
      int rc = fgetc(input);
      if (rc == EOF)
      {
        throw std::runtime_error("TGA: unexpected EOF (RGBA)");
      }

      size_t pixCount = (rc & 0x7F) + 1;

      if ((rc & 0x80) != 0) // RLE-packet
      {
        uint32_t copyPixel;
        if (fread(&copyPixel, 4, 1, input) != 1)
        {
          throw std::runtime_error("TGA: fread RLE failed (RGBA)");
        }
        while (pixCount-- > 0)
        {
          memcpy(cursor, &copyPixel, sizeof(uint32_t));
          cursor += 4;
        }
      }
      else // RAW-packet
      {
        if (fread(cursor, 4, pixCount, input) != pixCount)
        {
          throw std::runtime_error("TGA: fread raw failed (RGBA)");
        }
        cursor += 4 * pixCount;
      }
    }
  }

  void tgaRLEReadBW(uint8_t *data, const tgaHeader head, FILE *input)
  {
    uint8_t *cursor = data;

    while (cursor - data < head.is.width * head.is.height * 4)
    {
      int rc = fgetc(input);
      if (rc == EOF)
      {
        throw std::runtime_error("TGA: unexpected EOF (BW)");
      }

      size_t pixCount = (rc & 0x7F) + 1;

      if ((rc & 0x80) != 0) // RLE-packet
      {
        uint8_t copyPixel[2];
        if (fread(copyPixel, 2, 1, input) != 1)
        {
          throw std::runtime_error("TGA: fread RLE failed (BW)");
        }
        while (pixCount-- > 0)
        {
          cursor[0] = copyPixel[0];
          cursor[1] = copyPixel[0];
          cursor[2] = copyPixel[0];
          cursor[3] = copyPixel[1];
          cursor += 4;
        }
      }
      else // RAW-packet
      {
        while (pixCount-- > 0)
        {
          uint8_t copyPixel[2];
          if (fread(copyPixel, 2, 1, input) != 1)
          {
            throw std::runtime_error("TGA: fread RAW failed (BW)");
          }
          cursor[0] = copyPixel[0];
          cursor[1] = copyPixel[0];
          cursor[2] = copyPixel[0];
          cursor[3] = copyPixel[1];
          cursor += 4;
        }
      }
    }
  }

} // namespace

namespace bb
{

  image_t LoadTGA(const std::string &filename)
  {
    FILE *input = fopen(filename.c_str(), "rb");
    if (input == nullptr)
    {
      throw std::runtime_error(std::string("TGA: image not found") + filename);
    }
    BB_DEFER(fclose(input));

    if (fseek(input, -static_cast<long>(sizeof(tgaFooter)), SEEK_END) != 0)
    {
      throw std::runtime_error("TGA: fseek failed");
    }

    tgaFooter foot;

    if (fread(&foot, sizeof(tgaFooter), 1, input) != 1)
    {
      throw std::runtime_error("TGA: fread failed");
    }

    if (strcmp(foot.sig, TGA_SIGNATURE) != 0)
    {
      throw std::runtime_error("TGA: invalid signature");
    }

    clearerr(input);
    if (fseek(input, 0, SEEK_SET) != 0)
    {
      throw std::runtime_error("TGA: fseek failed");
    }

    tgaHeader head;

    if (fread(&head, sizeof(tgaHeader), 1, input) != 1)
    {
      throw std::runtime_error("TGA: fread failed");
    }

    if (head.idlen != 0)
    {
      throw std::runtime_error("TGA: invalid idlen");
    }

    if (head.cmtype != 0)
    {
      throw std::runtime_error("TGA: invalid cmtype");
    }

    auto colorMode = tgaGetColorMode(head.imtype);
    if (colorMode == tgaColorMode_t::unknown)
    {
      throw std::runtime_error("TGA: only RGB and BW images supported");
    }

    if (head.cms.size != 0)
    {
      throw std::runtime_error("TGA: invalid cms.size");
    }

    switch (colorMode)
    {
      case tgaColorMode_t::rgba:
        if (head.is.depth != 32)
        {
          throw std::runtime_error("TGA: invalid bit depth for RGB (32 expected)");
        }
        break;
      case tgaColorMode_t::bw:
        if (head.is.depth != 16)
        {
          throw std::runtime_error("TGA: invalid bit depth for BW (16 expected)");
        }
        break;
      default:
        // programmer error, can't reach here
        assert(0);
    }

    std::unique_ptr<uint8_t[]> pixels;

    pixels.reset(new uint8_t[head.is.width * head.is.height * 4]); // always allocate full 32-bit image

    if (!tgaIsRLE(head.imtype))
    {
      switch (colorMode)
      {
        case tgaColorMode_t::rgba:
        {
          size_t expectRead = head.is.width * head.is.height;
          size_t totalRead = fread(
            pixels.get(), 4, expectRead, input);
          if (totalRead != expectRead)
          {
            throw std::runtime_error("TGA: not enough RGB data");
          }
        }
        break;
        case tgaColorMode_t::bw:
        {
          size_t expectRead = head.is.width * head.is.height;
          uint8_t *cursor = pixels.get();
          uint8_t tmpPixel[2];
          while (expectRead-- > 0)
          {
            size_t totalRead = fread(tmpPixel, 2, 1, input);
            if (totalRead != 1)
            {
              throw std::runtime_error("TGA: fread failed BW");
            }
            cursor[0] = tmpPixel[0];
            cursor[1] = tmpPixel[0];
            cursor[2] = tmpPixel[0];
            cursor[3] = tmpPixel[1];
            cursor += 4;
          }
        }
        break;
        default:
          // can't reach here.
          assert(0);
      }
    }
    else
    {
      switch (colorMode)
      {
        case tgaColorMode_t::rgba:
          tgaRLEReadRGBA(pixels.get(), head, input);
          break;
        case tgaColorMode_t::bw:
          tgaRLEReadBW(pixels.get(), head, input);
          break;
        default:
          // can't reach here.
          assert(0);
      }
    }

    auto bgra = pixels.get();

    for (int index = 0; index < head.is.width * head.is.height; ++index)
    {
      uint8_t temp = bgra[index * 4];
      bgra[index * 4] = bgra[index * 4 + 2];
      bgra[index * 4 + 2] = temp;
    }

    if (head.is.orig == 0)
    {
      // image origin in lower left corner
      // so loader must invert line order
      std::unique_ptr<uint8_t[]> tempLine;
      tempLine.reset(new uint8_t[head.is.width * 4]);

      size_t byteWidth = head.is.width * 4;

      for (uint32_t line = 0; line < static_cast<uint32_t>(head.is.height) / 2; ++line)
      {
        memcpy(tempLine.get(), pixels.get() + line * byteWidth, byteWidth);
        memcpy(pixels.get() + line * byteWidth, pixels.get() + (head.is.height - line - 1) * byteWidth, byteWidth);
        memcpy(pixels.get() + (head.is.height - line - 1) * byteWidth, tempLine.get(), byteWidth);
      }
    }

    return bb::image_t(std::move(pixels), head.is.width, head.is.height, 4);
  }

} // namespace bb
