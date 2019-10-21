#include <cstring>
#include <cstdlib>
#include <stdexcept>
#include <cstdio>

#include <common.hpp>
#include <image.hpp>

namespace
{

  enum tgaErrors {
    TGA_OK  = 0, /**< No errors */
    TGA_IO  = 1, /**< IO error  */
    TGA_SIG = 2
  };

#pragma pack(push, 1)

  struct tgaColorMapSpec 
  {
    uint16_t first;
    uint16_t len;
    uint8_t  size;
  };
  
  struct tgaImageSpec
  {
    uint16_t xorig;
    uint16_t yorig;
    uint16_t width;
    uint16_t height;
    uint8_t  depth;
    uint8_t  alpha:4; // 3-0 
    uint8_t  _zero:1; // 4
    uint8_t  orig:1;  // 5
    uint8_t  _res:2;
  };
  
  struct tgaHeader
  {
    uint8_t         idlen;
    uint8_t         cmtype;
    uint8_t         imtype;
    tgaColorMapSpec cms;
    tgaImageSpec    is;
  };
  
  struct tgaFooter
  {
    uint32_t extoff;
    uint32_t devoff;
    char     sig[18];
  };

#pragma pack(pop)

const char TGA_SIGNATURE[] = "TRUEVISION-XFILE.";

  bool tgaIsTrueColor(uint8_t imtype)
  {
    return (imtype & 0x3) == 0x2;
  }

  bool tgaIsRLE(uint8_t imtype)
  {
    return (imtype & 0x8) == 0x8;
  }

} // namespace

namespace bb
{

  image_t LoadTGA(const std::string& filename)
  {
    FILE* input = fopen(filename.c_str(), "rb");
    if (input == nullptr)
    {
      throw std::runtime_error(std::string("TGA: image not found") + filename);
    }
    BB_DEFER(fclose(input));
    
    if (fseek(input, -sizeof(tgaFooter), SEEK_END) != 0) 
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
    if (fseek(input, 0, SEEK_SET) != 0) {
      throw std::runtime_error("TGA: fseek failed");
    }

    tgaHeader head;

    if (fread(&head, sizeof(tgaHeader), 1, input) != 1) {
      throw std::runtime_error("TGA: fread failed");
    }

    if (head.idlen != 0) 
    {
      throw std::runtime_error("TGA: invalid idlen");
    }

    if (head.cmtype != 0) {
      throw std::runtime_error("TGA: invalid cmtype");
    }

    if (!tgaIsTrueColor(head.imtype))
    {
      throw std::runtime_error("TGA: only TrueColor images supported");
    }

    if (head.cms.size != 0) {
      throw std::runtime_error("TGA: invalid cms.size");
    }

    if (head.is.depth != 32) {
      throw std::runtime_error("TGA: invalid bit depth");
    }

    std::unique_ptr<uint8_t[]> pixels;

    pixels.reset(new uint8_t[head.is.width*head.is.height*(head.is.depth/8)]);

    if (!tgaIsRLE(head.imtype))
    {
      size_t expectRead = head.is.width*head.is.height;
      size_t totalRead = fread(
        pixels.get(), 4, expectRead, input
      );
      if (totalRead  != expectRead) 
      {
        throw std::runtime_error("TGA: fread failed");
      }
    }
    else
    {
      auto cursor = pixels.get();
      while (cursor - pixels.get() < head.is.width*head.is.height*4)
      {
        int rc = fgetc(input);
        if (rc == EOF)
        {
          throw std::runtime_error("TGA: unexpected EOF");
        }
  
        size_t pixCount = (rc & 0x7F) + 1;
  
        if ((rc & 0x80) != 0) // RLE-packet
        {
          uint32_t copyPixel;
          if (fread(&copyPixel, 4, 1, input) != 1)
          {
            throw std::runtime_error("TGA: fread failed");
          }
          while(pixCount-->0)
          {
            memcpy(cursor, &copyPixel, sizeof(uint32_t));
            cursor += 4;
          }
        }
        else // RAW-packet
        {
          if (fread(cursor, 4, pixCount, input) != pixCount)
          {
            throw std::runtime_error("TGA: fread failed");
          }
          cursor += 4*pixCount;
        }
      }
    }

    auto bgra = pixels.get();

    for (int index = 0; index < head.is.width*head.is.height; ++index) {
      uint8_t temp = bgra[index * 4];
      bgra[index * 4] = bgra[index * 4 + 2];
      bgra[index * 4 + 2] = temp;
    }

    if (head.is.orig == 0) {
      // image origin in lower left corner
      // so loader must invert line order
      std::unique_ptr<uint8_t[]> tempLine;
      tempLine.reset(new uint8_t[head.is.width*head.is.depth/8]);

      size_t byteWidth = head.is.width*head.is.depth/8;

      for (int line = 0; line < head.is.height/2; ++line) {
        memcpy(tempLine.get(), pixels.get() + line*byteWidth, byteWidth);
        memcpy(pixels.get() + line*byteWidth, pixels.get() + (head.is.height - line - 1)*byteWidth, byteWidth);
        memcpy(pixels.get() + (head.is.height - line - 1)*byteWidth, tempLine.get(), byteWidth);
      }
    }

    return bb::image_t(std::move(pixels), head.is.width, head.is.height, head.is.depth/8);
  }

} // namespace draw