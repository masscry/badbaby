#include <iconv.h>

#include <common.hpp>
#include <config.hpp>
#include <font.hpp>

namespace
{

  using namespace bb;

  std::shared_ptr<texture_t> LoadTexture(const config_t& config)
  {
    // First option - just font texture filename given
    std::string fontTextureName = config.Value("font.texture", "");
    if (!fontTextureName.empty())
    {
      return std::shared_ptr<texture_t>(new texture_t(texture_t::LoadTGA(fontTextureName)));
    }

    // Second option - texture config filename given
    fontTextureName = config.Value("font.texture.config", "");
    if (!fontTextureName.empty())
    {
      return std::shared_ptr<texture_t>(new texture_t(texture_t::LoadConfig(fontTextureName)));
    }

    // Third option - try to find texture config in this config
    return std::shared_ptr<texture_t>(new texture_t(texture_t::LoadConfig(config)));
  }

} // namespace

namespace bb
{

  void font_t::Load(const std::string& filename)
  {
    config_t fontConfig;
    fontConfig.Load(filename);

    // type 0 - monospaced font
    if (fontConfig.Value("font.type", 0.0) != 0.0)
    {
      throw std::runtime_error("Unsupported font type");
    }

    this->width = fontConfig.Value("font.width", 1.0);
    this->height = fontConfig.Value("font.height", 1.0);

    // additional space between characters
    double xstride = fontConfig.Value("font.xstride", 0.0);
    double ystride = fontConfig.Value("font.ystride", 0.0);

    this->texture = LoadTexture(fontConfig);

    // each character smaller on size of stride
    const auto size = bb::vec2_t(1.0f/this->width - xstride, 1.0f/this->height - ystride);

    // class expect characters in WCHAR_T format, but they may be 
    // in some strange fasion arranged in texture.

    // this class support arrangement based on character table
    this->encoding = fontConfig.Value("font.encoding", "ASCII");

    iconv_t code;
    code = iconv_open("WCHAR_T", this->encoding.c_str());
    if (code == (iconv_t) -1)
    {
      throw std::runtime_error(std::string("Unsupported encoding ") + this->encoding);
    }
    BB_DEFER(iconv_close(code));

    for (uint8_t smb = 0; smb < std::numeric_limits<uint8_t>::max(); ++smb)
    {
      char input[1] = { static_cast<char>(smb) };
      char* inputCursor = input;
      size_t inputLeft = 1;

      uint32_t output[1];
      char* outputCursor = reinterpret_cast<char*>(output);
      size_t outputLeft = 4;

      size_t outputSize = iconv(code, &inputCursor, &inputLeft, &outputCursor, &outputLeft);
      if (outputSize == ((size_t)-1))
      {
        continue;
      }

      auto smbPos = bb::vec2_t(smb%this->width, smb/this->width);

      this->offsets[output[0]] = vec2_t { smbPos.x * (size.x + xstride), smbPos.y * (size.y + ystride) };
      this->sizes[output[0]] = size;
    }
  }

}