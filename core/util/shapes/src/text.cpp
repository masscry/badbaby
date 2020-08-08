#include <cstdarg>

#include <text.hpp>
#include <utf8.hpp>
#include <common.hpp>

namespace
{

  template<typename text_t>
  uint32_t GetTotalSpaceCount(const text_t& text)
  {
    uint32_t result = 0;
    for (auto smb: text)
    { // standard says, that isspace works for symbols between 0 and 255
      // otherwise - result implementation dependent
      // win32 crt raises assert here, so additional check here
      result += ((smb <= 0xFF) && (std::isspace(static_cast<int>(smb)) != 0));
    }
    return result;
  }

  const glm::vec2 uvMatrix[] = {
    { 0.0f, 1.0f },
    { 1.0f, 1.0f },
    { 0.0f, 0.0f },
    { 1.0f, 0.0f }
  };

  const glm::vec2 uvInvertedMatrix[] = {
    { 0.0f, 0.0f },
    { 1.0f, 0.0f },
    { 0.0f, 1.0f },
    { 1.0f, 1.0f }
  };

  static_assert(bb::countof(uvMatrix) == bb::countof(uvInvertedMatrix), "Both matricies expected to be same size!");
  static_assert(bb::countof(uvMatrix) == 4, "and size must be equal to 4, not 3, not 5, 4 is the number");

  size_t MakeText(
    const bb::font_t& font,
    const char* text,
    bb::vec2_t chSize,
    bb::textStorage_t& output)
  {
    auto symbols = bb::utf8extract(text);
    auto totalSpaces = GetTotalSpaceCount(symbols);

    auto totalSymbols = symbols.size() - totalSpaces;
    auto breakIndex = bb::breakingIndex<uint16_t>();

    assert(totalSymbols*4 < breakIndex); // in debug we assert this issue
    if (totalSymbols*4 >= breakIndex)
    { // we are using uint16_t for vert indecies
      // need to check if given text is to long for
      // in release we truncate tail
      symbols.resize(breakIndex/4);
    }

    if (output.vPos.size() < totalSymbols*4)
    {
      output.vPos.resize(totalSymbols*4);
      output.vUV.resize(totalSymbols*4);
      output.indecies.resize(totalSymbols*6);
    }

    assert(totalSymbols*6 <= output.indecies.size());

    uint32_t vID = 0;
    bb::vec3_t cursor = bb::vec3_t(0.0f);

    auto vPosIt = output.vPos.begin();
    auto vUVIt = output.vUV.begin();
    auto indIt = output.indecies.begin();

    const glm::vec2* pUVMatrix = (chSize.y < 0)?(uvInvertedMatrix):(uvMatrix);

    for (auto it: symbols)
    {
      if ((it <= 0xFF) && (std::isspace(static_cast<int>(it)) != 0))
      {
        switch (it)
        {
        case '\n':
          cursor.x = 0.0f;
          cursor.y += chSize.y;
          break;
        case '\t':
          // Move cursor integer part of tab stops in cursor + 1
          cursor.x = (floorf(cursor.x/(chSize.x*2.0f))+1.0f)*chSize.x*2.0f;
          break;
        default:
          bb::Warning("Unknown space char: (%02X)", it);
          /* FALLTHROUGH */
        case ' ':
          cursor.x += chSize.x;
        }
      }
      else
      {
        bb::vec2_t smbOffset = font.SymbolOffset(it);
        bb::vec2_t smbSize   = font.SymbolSize(it);

        *vPosIt++ = { cursor.x, cursor.y, cursor.z };
        *vPosIt++ = { cursor.x + chSize.x, cursor.y, cursor.z};
        *vPosIt++ = { cursor.x, cursor.y + chSize.y, cursor.z};
        *vPosIt++ = { cursor.x + chSize.x, cursor.y + chSize.y, cursor.z};

        *vUVIt++ = { smbOffset.x + smbSize.x * pUVMatrix[0].x, smbOffset.y + smbSize.y * pUVMatrix[0].y };
        *vUVIt++ = { smbOffset.x + smbSize.x * pUVMatrix[1].x, smbOffset.y + smbSize.y * pUVMatrix[1].y };
        *vUVIt++ = { smbOffset.x + smbSize.x * pUVMatrix[2].x, smbOffset.y + smbSize.y * pUVMatrix[2].y };
        *vUVIt++ = { smbOffset.x + smbSize.x * pUVMatrix[3].x, smbOffset.y + smbSize.y * pUVMatrix[3].y };

        *indIt++ = static_cast<uint16_t>(vID+0u);
        *indIt++ = static_cast<uint16_t>(vID+1u);
        *indIt++ = static_cast<uint16_t>(vID+2u);
        *indIt++ = static_cast<uint16_t>(vID+1u);
        *indIt++ = static_cast<uint16_t>(vID+3u);
        *indIt++ = static_cast<uint16_t>(vID+2u);
        vID += 4u;
        cursor.x += chSize.x;
      }
    }
    return totalSymbols*6;
  }

  using range_t = std::tuple<bb::utf8Symbols::iterator, bb::utf8Symbols::iterator>;

  bool ExtractLine(range_t input, size_t maxWidth, range_t* result)
  {
    assert(result != nullptr);

    auto cursor = std::get<0>(input);
    auto end = std::get<1>(input);

    auto start = cursor;

    bb::utf8Symbols::iterator lastSpace = end;
    bool newLine = false;

    for (size_t curWidth = 0; cursor != end; ++cursor, ++curWidth)
    {
      if (curWidth > maxWidth)
      {
        *result = std::make_tuple(start, lastSpace);
        return true;
      }

      switch (*cursor)
      {
      case '\n':
        if (newLine)
        { // break when two new lines reached
          *result = std::make_tuple(start, cursor);
          return true;
        }
        newLine = true;
        break;
      case ' ':
        lastSpace = cursor;
        break;
      default:
        newLine = false;
        break;
      }
    }

    *result = std::make_tuple(cursor, end);
    return (cursor != end);
  }

  size_t MakeTextMultiline(
    const bb::font_t& font,
    const std::string& text,
    bb::vec2_t chSize,
    bb::textStorage_t& output,
    size_t maxWidth)
  {
    auto symbols = bb::utf8extract(text.c_str());

    auto breakIndex = bb::breakingIndex<uint16_t>();

    assert(symbols.size()*4 < breakIndex); // in debug we assert this issue
    if (symbols.size()*4 >= breakIndex)
    { // we are using uint16_t for vert indecies
      // need to check if given text is to long for
      // in release we truncate tail
      symbols.resize(breakIndex/4);
    }

    if (output.vPos.size() < symbols.size()*4)
    {
      output.vPos.resize(symbols.size()*4);
      output.vUV.resize(symbols.size()*4);
      output.indecies.resize(symbols.size()*6);
    }

    assert(symbols.size()*6 <= output.indecies.size());

    uint32_t vID = 0;
    bb::vec3_t cursor = bb::vec3_t(0.0f);

    auto vPosIt = output.vPos.begin();
    auto vUVIt = output.vUV.begin();
    auto indIt = output.indecies.begin();

    bool newLine = true;

    range_t line = std::make_tuple(symbols.begin(), symbols.end());
    while(ExtractLine(line, maxWidth - (newLine*2), &line))
    {
      if (newLine)
      {
        cursor.x += chSize.x*2;
        newLine = false;
      }

      for (auto it = std::get<0>(line), e = std::get<1>(line); it != e; ++it)
      {
        switch(*it)
        {
          case '\n':
          case ' ':
            cursor.x += chSize.x;
            break;
          default:
          {
            bb::vec2_t smbOffset = font.SymbolOffset(*it);
            bb::vec2_t smbSize   = font.SymbolSize(*it);

            *vPosIt++ = { cursor.x, cursor.y, cursor.z };
            *vPosIt++ = { cursor.x + chSize.x, cursor.y, cursor.z};
            *vPosIt++ = { cursor.x, cursor.y + chSize.y, cursor.z};
            *vPosIt++ = { cursor.x + chSize.x, cursor.y + chSize.y, cursor.z};

            *vUVIt++ = { smbOffset.x, smbOffset.y + smbSize.y };
            *vUVIt++ = { smbOffset.x + smbSize.x, smbOffset.y + smbSize.y };
            *vUVIt++ = { smbOffset.x, smbOffset.y };
            *vUVIt++ = { smbOffset.x + smbSize.x, smbOffset.y };

            *indIt++ = static_cast<uint16_t>(vID+0u);
            *indIt++ = static_cast<uint16_t>(vID+1u);
            *indIt++ = static_cast<uint16_t>(vID+2u);
            *indIt++ = static_cast<uint16_t>(vID+1u);
            *indIt++ = static_cast<uint16_t>(vID+3u);
            *indIt++ = static_cast<uint16_t>(vID+2u);

            vID += 4u;
            cursor.x += chSize.x;
          }
        }
      }

      if ((std::get<1>(line) != symbols.end()) && (*std::get<1>(line) == '\n'))
      {
        newLine = true;
      }

      if (std::get<1>(line) == symbols.end())
      { // this happen when line has no spaces and whole line is processed
        break;
      }

      cursor.x = 0.0f;
      cursor.y -= chSize.y;

      line = std::make_tuple(std::get<1>(line)+1, symbols.end());
    }

    return symbols.size()*6;
  }

}

namespace bb
{

  void textStatic_t::Render()
  {
    assert(this->tex);
    texture_t::Bind(*this->tex);
    this->mesh.Render();
  }

  textStatic_t::textStatic_t()
  {
    ;
  }

  textStatic_t::textStatic_t(const font_t& font, const std::string& text, vec2_t chSize, size_t maxWidth)
  :tex(font.Texture())
  {
    textStorage_t textV;

    if (maxWidth == 0)
    {
      MakeText(font, text.c_str(), chSize, textV);
    }
    else
    {
      MakeTextMultiline(font, text, chSize, textV, maxWidth);
    }

    vbo_t vPosVBO = vbo_t::CreateArrayBuffer(textV.vPos.data(), ByteSize(textV.vPos), false);
    vbo_t vUVVBO = vbo_t::CreateArrayBuffer(textV.vUV.data(), ByteSize(textV.vUV), false);
    vbo_t indeciesVBO = vbo_t::CreateElementArrayBuffer(textV.indecies.data(), ByteSize(textV.indecies), false);

    vao_t vao = vao_t::CreateVertexAttribObject();

    vao.BindVBO(vPosVBO, 0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    vao.BindVBO(vUVVBO, 1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    vao.BindIndecies(indeciesVBO);

    this->mesh = mesh_t(std::move(vao), textV.indecies.size(), GL_TRIANGLES, 2);
  }

  void textDynamic_t::UpdateText(const char* text)
  {
    assert(this->font != nullptr);
    size_t textI = MakeText(*this->font, text, this->chSize, this->vertecies);

    if (textI == 0)
    {
      Error("%s", "Empty Dynamic String");
      assert(0);
      return;
    }

    if (this->totalI < textI)
    { // new bigger VBO needed
      Debug("Allocate New VBO (%lu)", textI);

      this->vPosVBO = vbo_t::CreateArrayBuffer(
        this->vertecies.vPos.data(),
        ByteSize(this->vertecies.vPos),
        true
      );
      this->vUVVBO = vbo_t::CreateArrayBuffer(
        this->vertecies.vUV.data(),
        ByteSize(this->vertecies.vUV),
        true
      );
      this->indeciesVBO = vbo_t::CreateElementArrayBuffer(
        this->vertecies.indecies.data(),
        ByteSize(this->vertecies.indecies),
        true
      );

      this->totalI = textI;
      this->totalV = this->vertecies.vPos.size();
    }
    else
    { // old VBO enough
      size_t textV = textI/6*4;

      this->vPosVBO.Update(0, sizeof(vec3_t)*textV, this->vertecies.vPos.data());
      this->vUVVBO.Update(0,  sizeof(vec2_t)*textV, this->vertecies.vUV.data());
      this->indeciesVBO.Update(0, sizeof(uint16_t)*textI, this->vertecies.indecies.data());
    }

    this->vao.BindVBO(vPosVBO, 0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    this->vao.BindVBO(vUVVBO, 1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    this->vao.BindIndecies(indeciesVBO);

    this->renderI = textI;
  }

  void textDynamic_t::Update(const std::string& text)
  {
    this->UpdateText(text.c_str());
  }

  void textDynamic_t::Update(const char* format, ...)
  {
    char* text = nullptr;
    va_list vl;
    va_start(vl, format);
    BB_DEFER(va_end(vl));

    auto len = vasprintf(&text, format, vl);
    if (len == -1)
    {
      bb::Error("%s", "vasprintf failed");
      return;
    }
    BB_DEFER(free(text));
    this->UpdateText(text);
  }

  void textDynamic_t::Render()
  {
    if (!this->vertecies.indecies.empty())
    {
      assert(this->font != nullptr);

      texture_t::Bind(*this->font->Texture());
      vao_t::Bind(this->vao);

      glEnableVertexAttribArray(0);
      glEnableVertexAttribArray(1);
      glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(this->renderI), GL_UNSIGNED_SHORT, 0);
      glDisableVertexAttribArray(0);
      glDisableVertexAttribArray(1);
    }
  }

  textDynamic_t::textDynamic_t()
  : font(nullptr),
    totalV(0),
    totalI(0),
    renderI(0)
  {
    ;
  }

  textDynamic_t::textDynamic_t(const font_t& font, const vec2_t& chSize)
  : vao(vao_t::CreateVertexAttribObject()),
    font(&font),
    chSize(chSize),
    totalV(0),
    totalI(0),
    renderI(0)
  {
    ;
  }

} // namespace bb
