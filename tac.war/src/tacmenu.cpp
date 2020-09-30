#include <tacwar.hpp>
#include <utf8.hpp>

namespace tac
{

  const char* textTitle = "TacWar";

  const char* textLines[menu_t::total] = {
    "Новая Игра",
    "Выход"
  };

  void menu_t::OnAction(int action)
  {
    switch(action)
    {
      case menu_t::newGame:
        bb::postOffice_t::Instance().Post(
          "tacwar",
          bb::Issue<bb::msg::dataMsg_t<tac::state_t>>(tac::state_t::game, -1)
        );
        break;
      case menu_t::exit:
        bb::postOffice_t::Instance().Post(
          "tacwar",
          bb::IssuePoison()
        );
        break;
      default:
        bb::Error("Unknown action: %d\n", action);
    }
  }

  void menu_t::OnClick()
  {
    auto& context = bb::context_t::Instance();
    auto curPos = context.MousePos();

    curPos.y = context.Height() - curPos.y;

    auto wPos = glm::unProjectZO(
      glm::vec3(curPos, 1.0f),
      glm::mat4(1.0f),
      this->camera.Projection(),
      glm::vec4(
        0.0f, 0.0f, context.Width(), context.Height()
      )
    );

    for (size_t i = 0; i < menu_t::total; ++i)
    {
      if ((wPos.y >= this->boundBox[i].x) && (wPos.y <= this->boundBox[i].y))
      {
        bb::postOffice_t::Instance().Post(
          "tacwar",
          bb::Issue<bb::msg::dataMsg_t<int>>(i, -1)
        );
      }
    }
  }

  void menu_t::OnUpdate(double dt)
  {
    auto& context = bb::context_t::Instance();
    auto curPos = context.MousePos();

    curPos.y = context.Height() - curPos.y;

    auto wPos = glm::unProjectZO(
      glm::vec3(curPos, 1.0f),
      glm::mat4(1.0f),
      this->camera.Projection(),
      glm::vec4(
        0.0f, 0.0f, context.Width(), context.Height()
      )
    );

    for (size_t i = 0; i < menu_t::total; ++i)
    {
      if ((wPos.y >= this->boundBox[i].x) && (wPos.y <= this->boundBox[i].y))
      {
        this->lineCols[i].y = 1.3f;
      }
      else
      {
        this->lineCols[i].y = 0.7f;
      }
    }
  }

  void menu_t::OnRender()
  {
    auto& context = bb::context_t::Instance();

    bb::framebuffer_t::Bind(context.Canvas());
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    bb::shader_t::Bind(this->lineShader);
    this->lineShader.SetBlock(
      "camera",
      this->camera.UniformBlock()
    );

    this->lineShader.SetVector4f(
      "lineColor",
      glm::vec4(1.3f, 1.3f, 1.3f, 1.0f)
    );

    this->title.Render();

    for (size_t i = 0; i < menu_t::total; ++i)
    {
      this->lineShader.SetVector4f(
        "lineColor",
        this->lineCols[i]
      );
      this->lines[i].Render();
    }

  }

  float CenterTextOffset(const char* text, float screenWidth)
  {
    auto len = bb::utf8len(text);

    auto xCenter = (screenWidth - len)*0.5f;
    return xCenter;
  }

  void menu_t::Prepare()
  {
    this->lineShader = bb::shader_t::LoadProgramFromFiles(
      "vec.vp.glsl", "vec.fp.glsl"
    );

    auto aspect = bb::context_t::Instance().AspectRatio();
    this->camera = bb::camera_t::Orthogonal(
      0.0f, 20.0f*aspect, 20.0f, 0.0f
    );

    auto centerPos = glm::vec3(
      CenterTextOffset("TacWar", 20.0f*aspect),
      2.0f,
      0.0f
    );

    this->title = bb::GenerateMesh(
      bb::DefineNumber(centerPos, 0.15f, glm::vec2(1.0f), "TacWar")
    );

    centerPos += 10.0;
    for (int i = 0; i < menu_t::total; ++i)
    {
      centerPos.x = CenterTextOffset(textLines[i], 20.0f*aspect);

      this->lines.emplace_back(
          bb::GenerateMesh(
            bb::DefineNumber(centerPos, 0.15f, glm::vec2(1.0f), textLines[i])
        )
      );
      this->lineCols.emplace_back(
        0.3f, 0.7f, 0.3f, 1.0f
      );
      this->boundBox.emplace_back(
        centerPos.y - 0.3,
        centerPos.y + 1.3f
      );
      centerPos.y += 2.5;
    }
  }

  void menu_t::Cleanup()
  {
    this->lines.clear();
    this->lineShader = bb::shader_t();
  }

}