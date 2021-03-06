#include <common.hpp>
#include <print.hpp>
#include <context.hpp>
#include <frameTimer.hpp>

#include <shapes.hpp>
#include <meshDesc.hpp>
#include <camera.hpp>
#include <shader.hpp>
#include <mapGen.hpp>

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
          roundf(hmap.Data(x, y)*4.0f)/4.0f*2.0f
        }
      );
      vcol.emplace_back(
        glm::vec3(
          hmap.Data(x, y)
        )
      );
      vnorm.emplace_back(
        hmap.NormalAtPoint(glm::vec2(x, y), 2.0f)
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

#ifdef __USE_ORTHO_CAM__

  auto scrDims = context.Dimensions()/500.0f;

  auto camera = bb::camera_t::Orthogonal(
    -scrDims.x/2.0f, scrDims.x/2.0f,
    -scrDims.y/2.0f, scrDims.y/2.0f
  );

#else

  auto camera = bb::camera_t::Perspective(
    45.0f,
    context.AspectRatio(),
    0.1f,
    1000.0f
  );

#endif

  auto camPos = glm::vec3(
    12.7f, 12.7f, 0.0f
  );

  camera.View() = glm::lookAt(
    camPos + glm::vec3(2.0f, 2.0f, 2.0f),
    camPos,
    glm::vec3(0.0f, 0.0f, 1.0f)
  );

  camera.Update();

  bb::mesh_t unit;

  if(FILE* unitMesh = fopen("green.obj.msh", "rb"))
  {
    BB_DEFER(fclose(unitMesh));
    unit = bb::GenerateMesh(bb::meshDesc_t::Load(unitMesh));
  }

  auto worldShader = bb::shader_t::LoadProgramFromFiles(
    "world.vp.glsl",
    "world.fp.glsl"
  );

  auto unitShader = bb::shader_t::LoadProgramFromFiles(
    "obj2mesh.vp.glsl",
    "obj2mesh.fp.glsl"
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

  bb::frameTimer_t frameTimer;

  auto unitPos = glm::vec3(0.0f);

  while(context.Update())
  {
    auto deltaTime = frameTimer.Delta();

    bb::framebuffer_t::Bind(context.Canvas());
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    bb::shader_t::Bind(worldShader);
    worldShader.SetBlock(
      "camera",
      camera.UniformBlock()
    );
    plane.Render();

    if (context.IsCursorInside())
    {
      auto curPos = context.MousePos();
      float pixValue;
      glReadPixels(
        static_cast<int>(curPos.x),
        static_cast<int>(context.Height() - curPos.y), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &pixValue
      );

      auto worldPos = glm::unProject(
        glm::vec3(curPos.x, context.Height() - curPos.y, pixValue),
        camera.View(),
        camera.Projection(),
        glm::vec4(0.0f, 0.0f, context.Width(), context.Height())
      );

      context.Title(
        bb::Print(
          '[', worldPos.x, ',', worldPos.y, ',', worldPos.z, ']'
        )
      );

      unitPos.x = worldPos.x;
      unitPos.y = worldPos.y;
      unitPos.z = worldPos.z;
    }

    bb::shader_t::Bind(unitShader);
    unitShader.SetBlock(
      "camera",
      camera.UniformBlock()
    );

    unitShader.SetMatrix(
      "model",
      glm::scale(
        glm::rotate(
          glm::rotate(
            glm::translate(
              glm::mat4(1.0f),
              unitPos
            ),
            glm::radians(90.0f),
            glm::vec3(1.0f, 0.0f, 0.0f)
          ),
          glm::radians(135.0f),
          glm::vec3(0.0f, 1.0f, 0.0f)
        ),
        glm::vec3(0.5f)
      )
    );

    unit.Render();

    auto moveCam = glm::vec2{
      (context.IsKeyDown(GLFW_KEY_LEFT) - context.IsKeyDown(GLFW_KEY_RIGHT)),
      (context.IsKeyDown(GLFW_KEY_DOWN) - context.IsKeyDown(GLFW_KEY_UP))
    };

    if (glm::length(moveCam) != 0.0f)
    {
      auto v = bb::Dir(glm::radians(45.0f));

      moveCam = glm::mat2(
        v.y, -v.x,
        v.x,  v.y
      ) * moveCam;

      moveCam *= deltaTime*10.0f;
      camPos += glm::vec3(moveCam, 0.0f);

      camera.View() = glm::lookAt(
        camPos + glm::vec3(2.0f, 2.0f, 2.0f),
        camPos,
        glm::vec3(0.0f, 0.0f, 1.0f)
      );

      camera.Update();
    }
  }
  return 0;
}