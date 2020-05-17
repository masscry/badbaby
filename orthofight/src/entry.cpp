#include <common.hpp>
#include <context.hpp>

#include <shapes.hpp>
#include <meshDesc.hpp>
#include <camera.hpp>
#include <shader.hpp>
#include <mapGen.hpp>

class frameTimer_t
{
  double start;

public:
  frameTimer_t()
  : start(glfwGetTime())
  {
    ;
  }

  double Delta()
  {
    auto finish = glfwGetTime();
    auto delta = finish - this->start;

    bb::context_t::Instance().Title(
      std::to_string(delta)
    );

    this->start = finish;
    return delta;
  }

  ~frameTimer_t()
  {
    ;
  }
};

glm::vec3 NormalAtPoint(const bb::ext::heightMap_t& hmap, glm::vec2 pos)
{
  auto p0 = glm::vec3(pos, hmap.Sample(pos)*2.0f);

  auto p1 = glm::vec3(pos + glm::vec2(-1.0f,  0.0f), hmap.Sample(pos + glm::vec2(-1.0f,  0.0f))*2.0f);
  auto p2 = glm::vec3(pos + glm::vec2( 0.0f, -1.0f), hmap.Sample(pos + glm::vec2( 0.0f, -1.0f))*2.0f);
  auto p3 = glm::vec3(pos + glm::vec2( 1.0f,  0.0f), hmap.Sample(pos + glm::vec2( 1.0f,  0.0f))*2.0f);
  auto p4 = glm::vec3(pos + glm::vec2( 0.0f,  1.0f), hmap.Sample(pos + glm::vec2( 0.0f,  1.0f))*2.0f);

  auto v1 = p1 - p0;
  auto v2 = p2 - p0;
  auto v3 = p3 - p0;
  auto v4 = p4 - p0;

  auto v12 = glm::cross(v1, v2);
  auto v23 = glm::cross(v2, v3);
  auto v34 = glm::cross(v3, v4);
  auto v41 = glm::cross(v4, v1);

  return glm::normalize(
    v12 + v23 + v34 + v41
  );
}

bb::mesh_t Plane(const bb::ext::heightMap_t& hmap)
{
  bb::meshDesc_t desc;

  desc.SetDrawMode(GL_TRIANGLES);

  std::vector<glm::vec3> vpos;
  std::vector<glm::vec3> vcol;
  std::vector<glm::vec3> vnorm;
  std::vector<uint16_t> indecies;

  vpos.reserve(hmap.DataSize());
  vcol.reserve(hmap.DataSize());
  vnorm.reserve(hmap.DataSize());
  indecies.reserve(hmap.DataSize()*6);

  uint16_t index = 0;
  for (size_t y = 0; y < hmap.Height(); ++y)
  {
    for (size_t x = 0; x < hmap.Width(); ++x)
    {
      vpos.emplace_back(
        glm::vec3{
          static_cast<float>(x)/10.0f,
          static_cast<float>(y)/10.0f, 
          hmap.Data(x, y)*2.0f
        }
      );
      vcol.emplace_back(
        glm::vec3(
          hmap.Data(x, y)
        )
      );
      vnorm.emplace_back(
        NormalAtPoint(hmap, glm::vec2(x, y))
      );

      if ((y + 1 < hmap.Height()) && (x + 1 < hmap.Width()))
      {
        indecies.emplace_back(index);
        indecies.emplace_back(index + 1);
        indecies.emplace_back(index + hmap.Width());

        indecies.emplace_back(index + hmap.Width());
        indecies.emplace_back(index + 1);
        indecies.emplace_back(index + hmap.Width()+1);
        ++index;
      }
    }
    ++index;
    printf("\n");
  }

  desc.Buffers().emplace_back(
    bb::MakeVertexBuffer(std::move(vpos))
  );

  desc.Buffers().emplace_back(
    bb::MakeVertexBuffer(std::move(vcol))
  );

  desc.Buffers().emplace_back(
    bb::MakeVertexBuffer(std::move(vnorm))
  );

  desc.Indecies() = bb::MakeIndexBuffer(std::move(indecies));

  return bb::GenerateMesh(desc);
}

int main(int argc, char* argv[])
{
  if (bb::ProcessStartupArguments(argc, argv) != 0)
  {
    return -1;
  }

  auto& context = bb::context_t::Instance();

  auto scrDims = context.Dimensions()/500.0f;

  auto camera = bb::camera_t::Orthogonal(
    -scrDims.x, scrDims.x,
    -scrDims.y, scrDims.y
  );

  auto camPos = glm::vec3(
    0.0f, 0.0f, 0.0f
  );

  camera.View() = glm::lookAt(
    camPos + glm::vec3(0.5f, 0.5f, 0.5f),
    camPos,
    glm::vec3(0.0f, 0.0f, 1.0f)
  );

  auto worldShader = bb::shader_t::LoadProgramFromFiles(
    "world.vp.glsl",
    "world.fp.glsl"
  );

  auto world = bb::ext::MakeHMapUsingOctaves(
    bb::ext::generate_t(
      -1,
      255,
      255,
      0.5f,
      20.0f,
      0,
      0.6f,
      10,
      1.5
    )
  );

  auto plane = Plane(world);

  frameTimer_t frameTimer;
  while(context.Update())
  {
    camera.View() = glm::lookAt(
      camPos + glm::vec3(0.5f, 0.5f, 0.5f),
      camPos,
      glm::vec3(0.0f, 0.0f, 1.0f)
    );

    camera.Update();

    bb::framebuffer_t::Bind(context.Canvas());
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    bb::shader_t::Bind(worldShader);
    worldShader.SetBlock(
      worldShader.UniformBlockIndex("camera"),
      camera.UniformBlock()
    );
    plane.Render();

    auto moveCam = glm::vec2{
      (context.IsKeyDown(GLFW_KEY_LEFT) - context.IsKeyDown(GLFW_KEY_RIGHT)),
      (context.IsKeyDown(GLFW_KEY_DOWN) - context.IsKeyDown(GLFW_KEY_UP))
    };

    float sinv;
    float cosv;

    sincosf(glm::radians(45.0f), &sinv, &cosv);

    moveCam = glm::mat2(
      cosv, -sinv,
      sinv, cosv
    ) * moveCam;

    moveCam *= frameTimer.Delta()*10.0f;
    camPos += glm::vec3(moveCam, 0.0f);
  }
  return 0;
}