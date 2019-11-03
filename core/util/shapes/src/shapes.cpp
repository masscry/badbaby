#include <shapes.hpp>
#include <utility>

namespace
{

  const glm::vec2 vPos[4] = {
    { -0.5f, -0.5f },
    { +0.5f, -0.5f },
    { +0.5f, +0.5f },
    { -0.5f, +0.5f}
  };

  const glm::vec2 vUV[4] = {
    { 0.0f, 1.0f },
    { 1.0f, 1.0f },
    { 1.0f, 0.0f },
    { 0.0f, 0.0f }
  };

  const uint16_t vInd[6] = {
    0, 1, 2, 0, 2, 3
  };

}

namespace bb
{

  void plane_t::Render()
  {
    bb::vao_t::Bind(this->vao);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
  }

  plane_t::plane_t(glm::vec2 size)
  {
    glm::vec2 xPos[4] = {
      vPos[0]*size,
      vPos[1]*size,
      vPos[2]*size,
      vPos[3]*size
    };

    auto vboPos = bb::vbo_t::CreateArrayBuffer(xPos, sizeof(xPos), false);
    auto vboUV  = bb::vbo_t::CreateArrayBuffer(vUV,  sizeof(vUV),  false);
    auto vboInd = bb::vbo_t::CreateElementArrayBuffer(vInd, sizeof(vInd), false);
    auto vao    = bb::vao_t::CreateVertexAttribObject();

    vao.BindVBO(vboPos, 0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    vao.BindVBO(vboUV,  1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    vao.BindIndecies(vboInd);

    this->vao = std::move(vao);
  }

}