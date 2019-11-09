#include <text.hpp>
#include <utf8.hpp>

namespace
{

  void MakeText(
    const bb::font_t& font,
    const std::string& text,
    bb::vec2_t chSize,
    bb::textStorage_t& output)
  {
    auto symbols = bb::utf8extract(text.c_str());

    output.vPos.clear();
    output.vUV.clear();
    output.indecies.clear();

    output.vPos.reserve(symbols.size()*4);
    output.vUV.reserve(symbols.size()*4);
    output.indecies.reserve(symbols.size()*6);

    uint16_t vID = 0;
    bb::vec3_t cursor = bb::vec3_t(0.0f);

    for (auto it = symbols.begin(), e = symbols.end(); it != e; ++it)
    {
      bb::vec2_t smbOffset = font.SymbolOffset(*it);
      bb::vec2_t smbSize   = font.SymbolSize(*it);

      output.vPos.emplace_back(cursor.x, cursor.y, cursor.z);
      output.vUV.emplace_back(smbOffset.x, smbOffset.y + smbSize.y);

      output.vPos.emplace_back(cursor.x + chSize.x, cursor.y, cursor.z);
      output.vUV.emplace_back(smbOffset.x + smbSize.x, smbOffset.y + smbSize.y);

      output.vPos.emplace_back(cursor.x, cursor.y + chSize.y, cursor.z);
      output.vUV.emplace_back(smbOffset.x, smbOffset.y);

      output.vPos.emplace_back(cursor.x + chSize.x, cursor.y + chSize.y, cursor.z);
      output.vUV.emplace_back(smbOffset.x + smbSize.x, smbOffset.y);

      output.indecies.push_back(vID+0);
      output.indecies.push_back(vID+1);
      output.indecies.push_back(vID+2);
      output.indecies.push_back(vID+1);
      output.indecies.push_back(vID+3);
      output.indecies.push_back(vID+2);

      vID += 4;
      cursor.x += chSize.x;
    }
  }

}

namespace bb
{

  void textStatic_t::Render()
  {
    texture_t::Bind(*this->tex);
    this->mesh.Render();
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
    textStorage_t textV;
    MakeText(this->font, text, this->chSize, textV);

    if (textV.indecies.size() > this->vertecies.indecies.size())
    {
      this->vPosVBO = vbo_t::CreateArrayBuffer(textV.vPos.data(), ByteSize(textV.vPos), true);
      this->vUVVBO = vbo_t::CreateArrayBuffer(textV.vUV.data(), ByteSize(textV.vUV), true);
      this->indeciesVBO = vbo_t::CreateElementArrayBuffer(textV.indecies.data(), ByteSize(textV.indecies), true);

      this->vao.BindVBO(vPosVBO, 0, 3, GL_FLOAT, GL_FALSE, 0, 0);
      this->vao.BindVBO(vUVVBO, 1, 2, GL_FLOAT, GL_FALSE, 0, 0);
      this->vao.BindIndecies(indeciesVBO);
    }
    else
    {
      this->vPosVBO.Update(0, ByteSize(textV.vPos), textV.vPos.data());
      this->vUVVBO.Update(0, ByteSize(textV.vUV), textV.vUV.data());
      this->indeciesVBO.Update(0, ByteSize(textV.indecies), textV.indecies.data());
    }
    this->vertecies = std::move(textV);
  }

  void textDynamic_t::Render()
  {
    if (!this->vertecies.indecies.empty())
    {
      texture_t::Bind(*this->font.Texture());
      vao_t::Bind(this->vao);

      glEnableVertexAttribArray(0);
      glEnableVertexAttribArray(1);
      glDrawElements(GL_TRIANGLES, this->vertecies.indecies.size(), GL_UNSIGNED_SHORT, 0);
      glDisableVertexAttribArray(0);
      glDisableVertexAttribArray(1);
    }
  }

  textDynamic_t::textDynamic_t(const font_t& font, const vec2_t& chSize)
  :vao(vao_t::CreateVertexAttribObject()),font(font),chSize(chSize)
  {
    ;
  }

} // namespace bb
