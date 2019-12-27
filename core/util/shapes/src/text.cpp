#include <text.hpp>
#include <utf8.hpp>
#include <common.hpp>

namespace
{

  size_t MakeText(
    const bb::font_t& font,
    const std::string& text,
    bb::vec2_t chSize,
    bb::textStorage_t& output)
  {
    auto symbols = bb::utf8extract(text.c_str());

    if (output.vPos.size() < symbols.size()*4)
    {
      output.vPos.resize(symbols.size()*4);
      output.vUV.resize(symbols.size()*4);
      output.indecies.resize(symbols.size()*6);
    }

    assert(symbols.size()*6 <= output.indecies.size());

    uint16_t vID = 0;
    bb::vec3_t cursor = bb::vec3_t(0.0f);

    auto vPosIt = output.vPos.begin();
    auto vUVIt = output.vUV.begin();
    auto indIt = output.indecies.begin();

    for (auto it = symbols.begin(), e = symbols.end(); it != e; ++it)
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

      *indIt++ = vID+0;
      *indIt++ = vID+1;
      *indIt++ = vID+2;
      *indIt++ = vID+1;
      *indIt++ = vID+3;
      *indIt++ = vID+2;

      vID += 4;
      cursor.x += chSize.x;
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

  textStatic_t::textStatic_t(const font_t& font, const std::string& text, vec2_t chSize)
  :tex(font.Texture())
  {
    textStorage_t textV;
    MakeText(font, text, chSize, textV);

    vbo_t vPosVBO = vbo_t::CreateArrayBuffer(textV.vPos.data(), ByteSize(textV.vPos), false);
    vbo_t vUVVBO = vbo_t::CreateArrayBuffer(textV.vUV.data(), ByteSize(textV.vUV), false);
    vbo_t indeciesVBO = vbo_t::CreateElementArrayBuffer(textV.indecies.data(), ByteSize(textV.indecies), false);

    vao_t vao = vao_t::CreateVertexAttribObject();

    vao.BindVBO(vPosVBO, 0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    vao.BindVBO(vUVVBO, 1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    vao.BindIndecies(indeciesVBO);

    this->mesh = mesh_t(std::move(vao), textV.indecies.size());
  }

  void textDynamic_t::Update(const std::string& text)
  {
    assert(this->font != nullptr);
    size_t textI = MakeText(*this->font, text, this->chSize, this->vertecies);

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

      this->vao.BindVBO(vPosVBO, 0, 3, GL_FLOAT, GL_FALSE, 0, 0);
      this->vao.BindVBO(vUVVBO, 1, 2, GL_FLOAT, GL_FALSE, 0, 0);
      this->vao.BindIndecies(indeciesVBO);

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

    this->renderI = textI;
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
      glDrawElements(GL_TRIANGLES, this->renderI, GL_UNSIGNED_SHORT, 0);
      glDisableVertexAttribArray(0);
      glDisableVertexAttribArray(1);
    }
  }

  textDynamic_t::textDynamic_t()
  : font(nullptr),
    totalV(0),
    totalI(0)
  {
    ;
  }

  textDynamic_t::textDynamic_t(const font_t& font, const vec2_t& chSize)
  :vao(vao_t::CreateVertexAttribObject()),font(&font),chSize(chSize),totalV(0),totalI(0)
  {
    ;
  }

} // namespace bb
