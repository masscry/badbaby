#include <common.hpp>
#include <context.hpp>

#include <meshDesc.hpp>
#include <shapes.hpp>

#include <cassert>

using namespace bb;

template<typename data_t>
class array2D_t final
{
  data_t* data;
  size_t width;
  size_t height;
public:

  bool IsInside(size_t x, size_t y)
  {
    return (x < this->width) && (y < this->height);
  }

  bool IsGood() const
  {
    return (this->data != nullptr);
  }

  size_t DataSize() const
  {
    return this->width*this->height;
  }

  size_t Width() const
  {
    return this->width;
  }

  size_t Height() const
  {
    return this->height;
  }

  data_t* Data()
  {
    return this->data;
  }

  const data_t* Data() const
  {
    return this->data;
  }

  data_t& operator()(size_t x, size_t y)
  {
    return this->data[y*this->width + x];
  }

  data_t operator()(size_t x, size_t y) const
  {
    return this->data[y*this->width + x];
  }

  array2D_t(size_t width, size_t height)
  : data(nullptr),
    width(width),
    height(height)
  {
    this->data = new(std::nothrow) data_t[this->width*this->height];
    if (this->data == nullptr)
    {
      Error("Can't allocate array (%lu %lu)", this->width, this->height);
      throw std::bad_alloc();
    }
  }

  array2D_t(size_t width, size_t height, data_t initVal)
  : data(nullptr),
    width(width),
    height(height)
  {
    this->data = new(std::nothrow) data_t[this->width*this->height];
    if (this->data == nullptr)
    {
      Error("Can't allocate array (%lu %lu)", this->width, this->height);
      throw std::bad_alloc();
    }

    std::fill(this->data, this->data + this->DataSize(), initVal);
  }

  array2D_t(const array2D_t<data_t>& src)
  : data(nullptr), 
    width(src.width),
    height(src.height)
  {
    if (src.IsGood())
    {
      this->data = new(std::nothrow) data_t[this->width*this->height];
      if (this->data == nullptr)
      {
        Error("Can't allocate array (%u %u)", this->width, this->height);
        throw std::bad_alloc();
      }
      std::copy(src.Data(), src.Data() + src.DataSize(), this->data);
    }
  }

  array2D_t& operator=(const array2D_t<data_t>& src)
  {
    if (this != &src)
    {
      if (src.IsGood())
      {
        if (this->DataSize() != src.DataSize())
        {
          delete[] this->data;
          this->data = new(std::nothrow) data_t[src.DataSize()];
          if (this->data == nullptr)
          {
            Error("Can't allocate array (%lu %lu)", this->width, this->height);
            throw std::bad_alloc();
          }
        }

        this->width = src.width;
        this->height = src.height;
        std::copy(src.Data(), src.Data() + src.DataSize(), this->data);
      }
      else
      {
        delete[] this->data;
        this->width = 0;
        this->height = 0;
      }
    }
    return *this;
  }

  ~array2D_t()
  {
    delete[] this->data;
  }

  array2D_t(array2D_t<data_t>&& src)
  : data(src.data),
    width(src.width),
    height(src.height)
  {
    src.data = nullptr;
    src.width = 0;
    src.height = 0;
  }

  array2D_t& operator=( array2D_t<data_t>&& src)
  {
    if (this != &src)
    {
      delete[] this->data;

      this->data = src.data;
      this->width = src.width;
      this->height = src.height;

      src.data = nullptr;
      src.width = 0;
      src.height = 0;
    }
    return *this;
  }

};

enum
{
  SELF = -3,
  ANY = -2,
  NONE = -1,
  EMPTY = 0,
  SAND,
  ROCK,
  WATER,
  TOTAL
};

struct rule_t
{
  char pattern[3][3];
  char rule[3][3];
};

rule_t sandRules[] = {
  {
    { 
      { NONE,  NONE, NONE },
      { NONE,  SELF, NONE },
      { NONE, EMPTY, NONE } 
    }, 
    { 
      { NONE,  NONE, NONE },
      { NONE, EMPTY, NONE },
      { NONE,  SELF, NONE } 
    } 
  },
  {
    {
      {  NONE, NONE, NONE },
      { EMPTY, SELF, NONE },
      { EMPTY,  ANY, NONE }
    },
    {
      { NONE,   NONE, NONE },
      { EMPTY, EMPTY, NONE },
      { SELF,    ANY, NONE }
    }
  },
  {
    {
      { NONE, NONE,  NONE },
      { NONE, SELF, EMPTY },
      { NONE,  ANY, EMPTY }
    },
    {
      { NONE, NONE,  NONE  },
      { NONE, EMPTY, EMPTY },
      { NONE,   ANY,  SELF }
    }
  },
  {
    {
      { NONE, NONE, NONE },
      { NONE, SELF, NONE },
      { NONE, NONE, NONE }
    },
    {
      { NONE, NONE, NONE },
      { NONE, SELF, NONE },
      { NONE, NONE, NONE }
    }
  }
};

rule_t waterRules[] = 
{
  {
    { 
      { NONE,  NONE, NONE },
      { NONE,  SELF, NONE },
      { NONE, EMPTY, NONE } 
    }, 
    { 
      { NONE,  NONE, NONE },
      { NONE, EMPTY, NONE },
      { NONE,  SELF, NONE } 
    } 
  },
  {
    {
      {  NONE, NONE, NONE },
      { EMPTY, SELF, NONE },
      { EMPTY,  ANY, NONE }
    },
    {
      { NONE,   NONE, NONE },
      { EMPTY, EMPTY, NONE },
      { SELF,    ANY, NONE }
    }
  },
  {
    {
      { NONE, NONE,  NONE },
      { NONE, SELF, EMPTY },
      { NONE,  ANY, EMPTY }
    },
    {
      { NONE, NONE,  NONE  },
      { NONE, EMPTY, EMPTY },
      { NONE,   ANY,  SELF }
    }
  },
  {
    {
      { NONE, NONE, NONE  },
      { NONE, SELF, EMPTY },
      {  ANY,  ANY, ANY   }
    },
    {
      { NONE, NONE,  NONE  },
      { NONE, EMPTY,  SELF },
      {  ANY,   ANY,   ANY }
    }
  },
  {
    {
      {  NONE,  NONE, NONE },
      { EMPTY,  SELF, NONE },
      {   ANY,   ANY,  ANY }
    },
    {
      { NONE,  NONE,  NONE },
      { SELF, EMPTY,  NONE },
      {  ANY,   ANY,   ANY }
    }
  },
  {
    { 
      { NONE,  SAND, NONE },
      { NONE,  SELF, NONE },
      { NONE,  NONE, NONE } 
    }, 
    { 
      { NONE,  SELF, NONE },
      { NONE,  SAND, NONE },
      { NONE,  NONE, NONE } 
    } 
  },
  {
    {
      { NONE, NONE, NONE },
      { NONE, SELF, NONE },
      { NONE, NONE, NONE }
    },
    {
      { NONE, NONE, NONE },
      { NONE, SELF, NONE },
      { NONE, NONE, NONE }
    }
  }
};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"

int ApplyRule(array2D_t<int>& world, size_t x, size_t y, const rule_t& rule)
{
  auto self = world(x, y);

  for (int dy = -1; dy <= 1; ++dy)
  {
    for (int dx = -1; dx <= 1; ++dx)
    {
      auto patCell = rule.pattern[1 + dy][1 + dx];
      if (patCell != NONE)
      {
        if (!world.IsInside(x + dx, y + dy))
        { // can't check this rule
          return -1;
        }
      }
      switch (patCell)
      {
        case NONE:
          continue;
        case SELF:
          if (world(x + dx , y + dy) != self)
          {
            return -1;
          }
          break;
        case ANY:
          if (world(x + dx , y + dy) == EMPTY)
          {
            return -1;
          }
          break;
        default:
          if (world(x + dx, y + dy) != patCell)
          {
            return -1;
          }
          break;
      }
    }
  }

  // here we know that rule must apply

  for (int dy = -1; dy <= 1; ++dy)
  {
    for (int dx = -1; dx <= 1; ++dx)
    {
      auto patCell = rule.rule[1 + dy][1 + dx];

      switch (patCell)
      {
      case NONE:
      case ANY:
        continue;
      case SELF:
        world(x + dx, y + dy) = self;
        break;
      default:
        world(x + dx, y + dy) = patCell;
      }
    }
  }

  return 0;
}

#pragma GCC diagnostic pop

void UpdateWorld(array2D_t<int>& world)
{
  auto mask = array2D_t<int>(world.Width(), world.Height(), 0);

  for (size_t line = 0; line < world.Height(); ++line)
  {
    size_t y =  world.Height() - (line+1);
    for (size_t x = 0; x < world.Width(); ++x)
    {
      switch(world(x, y))
      {
        case SAND:
          for (auto rule: sandRules)
          {
            if (ApplyRule(world, x, y, rule) == 0)
            {
              break;
            }
          }
          break;
        case WATER:
          for (auto rule: waterRules)
          {
            if (ApplyRule(world, x, y, rule) == 0)
            {
              break;
            }
          }
          break;
        default:
          break;
      }
    }
  }
}

const glm::vec3 frColors[TOTAL] = {
  glm::vec3(0.0f),
  glm::vec3(0.761f, 0.698f, 0.502f),
  glm::vec3(0.5f, 0.2f, 0.6f),
  glm::vec3(0.1f, 0.1f, 0.8f)
};

mesh_t BuildMesh(const array2D_t<int>& world)
{
  meshDesc_t meshDesc;

  std::vector<glm::vec2> pos;
  std::vector<glm::vec3> col;
  std::vector<uint16_t> indxs;

  pos.reserve(world.DataSize());
  col.reserve(world.DataSize());
  indxs.reserve(world.DataSize());

  uint32_t index = 0; 
  for (size_t y = 0; y < world.Height(); ++y)
  {
    for (size_t x = 0; x < world.Width(); ++x)
    {
      pos.emplace_back(
        x, y
      );

      auto worldXY = world(x,y);

      if ((worldXY >= EMPTY) && (worldXY < TOTAL))
      {
        col.emplace_back(frColors[worldXY]);
      }
      else
      {
        col.emplace_back(glm::vec3(0.6, 0.4, 0.8));
      }

      assert(index < 0x10000);

      indxs.emplace_back(index++);
    }
  }

  meshDesc.Buffers().emplace_back(
    MakeVertexBuffer(std::move(pos))
  );

  meshDesc.Buffers().emplace_back(
    MakeVertexBuffer(std::move(col))
  );

  meshDesc.Indecies() = MakeIndexBuffer(std::move(indxs));

  meshDesc.SetDrawMode(GL_POINTS);

  return GenerateMesh(meshDesc);
}

int main(int argc, char* argv[])
{
  if (ProcessStartupArguments(argc, argv) != 0)
  {
    return -1;
  }

  auto& context = context_t::Instance();
  context.SetStickyMouse(true);

  auto camera = bb::camera_t::Orthogonal(
    -0.5f,
    255.5f,
    255.5f,
    -0.5f
  );

  auto renderProgram = bb::shader_t::LoadProgramFromFiles(
    "auto.vp.glsl",
    "auto.fp.glsl"
  );
  const auto cameraBindPoint = renderProgram.UniformBlockIndex("camera"); 

  glDisable(GL_PRIMITIVE_RESTART);
  glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
  glPointSize(4.0f);

  auto world = array2D_t<int>(256, 256, EMPTY);
  auto worldMesh = mesh_t();

  int mode = 0;

  auto changeTimeout = 0.0;
  auto lastTick = glfwGetTime();
  while(context.Update())
  {
    auto nowTick = glfwGetTime();
    auto delta = nowTick - lastTick;
    lastTick = nowTick;

    changeTimeout += delta;

    if (context.IsCursorInside())
    {
      auto pos = context.MousePos() + glm::dvec2(0.5);
      pos /= 4;

      if (context.IsButtonPressed(GLFW_MOUSE_BUTTON_RIGHT) && (changeTimeout >= 0.5f)) 
      {
        mode = (mode + 1)%(TOTAL-1);
        changeTimeout = 0;
      }

      if (world(static_cast<size_t>(pos.x), static_cast<size_t>(pos.y)) == EMPTY)
      {
        if (context.IsButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
        {
          if ((pos.x < 256) && (pos.y < 256) && (pos.x >= 0) && (pos.y >= 0))
          {
            world(static_cast<size_t>(pos.x), static_cast<size_t>(pos.y)) = mode+1;
          }
        }
      }
    }

    UpdateWorld(world);
    worldMesh = BuildMesh(world);

    bb::framebuffer_t::Bind(context.Canvas());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (worldMesh.Good())
    {
      bb::shader_t::Bind(renderProgram);
      camera.Update();
      renderProgram.SetBlock(cameraBindPoint, camera.UniformBlock());

      worldMesh.Render();
    }

    context.Title(std::to_string(1.0/delta) + " " + std::to_string(mode+1));
  }

  return 0;
}