#include <common.hpp>
#include <algebra.hpp>
#include <shapes.hpp>
#include <context.hpp>
#include <camera.hpp>

#include <climits>
#include <cstring>
#include <unordered_map>
#include <vector>
#include <string>

#ifdef _WIN32

#include <getline.hpp>

#include <cstdlib>
#include <basetsd.h>

char* realpath(const char* path, char* abs_path)
{
  return _fullpath(abs_path, path, 0);
}

using ssize_t = SSIZE_T;

#endif


using mtlLib_t = std::unordered_map<std::string, glm::vec3>;
using vPos_t = std::vector<glm::vec3>;
using vInd_t = std::vector<uint16_t>;

#define EXIT_ERROR_MAT(TEXT) {\
  printf("%s:%zu: error: unknown command \"%s\"\n", absPathPtr, lineNum, (TEXT));\
  return -1;\
}

glm::vec3 MaxBox(const vPos_t& vPos)
{
  glm::vec3 result(std::numeric_limits<float>::lowest());
  for (auto pos: vPos)
  {
    result.x = (pos.x > result.x)?(pos.x):(result.x);
    result.y = (pos.y > result.y)?(pos.y):(result.y);
    result.z = (pos.z > result.z)?(pos.z):(result.z);
  }
  return result;
}

glm::vec3 MinBox(const vPos_t& vPos)
{
  glm::vec3 result(std::numeric_limits<float>::max());
  for (auto pos: vPos)
  {
    result.x = (pos.x < result.x)?(pos.x):(result.x);
    result.y = (pos.y < result.y)?(pos.y):(result.y);
    result.z = (pos.z < result.z)?(pos.z):(result.z);
  }
  return result;
}


int LoadMaterials(const char* matLibName, mtlLib_t* pMtlLib)
{
  if ((matLibName == nullptr) || (pMtlLib == nullptr))
  {
    return -1;
  }

  char* absPathPtr = realpath(matLibName, nullptr);
  if (absPathPtr == nullptr)
  {
    fprintf(stderr, "%s\n", "Error: Can't get absolute path for obj");
    return -1;
  }
  BB_DEFER(free(absPathPtr));

  printf("Materials: \"%s\"\n", absPathPtr);

  FILE* input = fopen(absPathPtr, "rt");
  if (input == NULL)
  {
    fprintf(stderr, "File \"%s\" not found.", absPathPtr);
    return -1;
  }
  BB_DEFER(fclose(input));

  mtlLib_t mtlLib;

  char* text = nullptr;
  size_t len = 0;
  ssize_t nread;
  BB_DEFER(free(text));

  size_t lineNum = 1;

  std::string matName;

  while ((nread = getline(&text, &len, input)) != -1)
  {
    if (text[0] == '\n')
    {
      ++lineNum;
      continue;
    }

    text = strtok(text, "\n");
    if (text == nullptr)
    {
      EXIT_ERROR_MAT("Invalid String");
    }

    switch (text[0])
    {
    case '#':
    case '\n':
      // skip comments and newlines
      break;
    case 'n':
      {
        char* token;
        token = strtok(text, " \n");
        if (strcmp(token, "newmtl") != 0)
        {
          EXIT_ERROR_MAT(token);
        }

        char* newMatName;
        newMatName = strtok(nullptr, " \n");
        if (newMatName == nullptr)
        {
          EXIT_ERROR_MAT("Invalid MTL format! Material name is not found.");
        }
        matName = newMatName;
      }
      break;
    case 'i':
    case 'N':
    case 'd':
      break;
    case 'K':
      {
        char* token;
        token = strtok(text, " \n");
        if (strcmp(token, "Kd") == 0)
        {
          glm::vec3 vColor;
          char* vData = strtok(nullptr, "\n");
          char* vNextData;

          errno = 0;
          vColor.x = strtof(vData, &vNextData);
          if ((errno != 0) || (vNextData == vData))
          {
            EXIT_ERROR_MAT("Bad Color Format R!");
          }
          vData = vNextData;

          errno = 0;
          vColor.y = strtof(vData, &vNextData);
          if ((errno != 0) || (vNextData == vData))
          {
            EXIT_ERROR_MAT("Bad Color Format G!");
          }
          vData = vNextData;

          errno = 0;
          vColor.z = strtof(vData, &vNextData);
          if ((errno != 0) || (vNextData == vData))
          {
            EXIT_ERROR_MAT("Bad Color Format B!");
          }
          vData = vNextData;

          if (matName.empty())
          {
            EXIT_ERROR_MAT("Invalid MTL Format! Material name is not found!");
          }

          if (mtlLib.find(matName) != mtlLib.end())
          {
            EXIT_ERROR_MAT("Invalid MTL Format! Material color already defined!");
          }
          mtlLib[matName] = vColor;
        }
      }
      break;
    default:
      EXIT_ERROR_MAT(text);
      break;
    }
    ++lineNum;
  }

  *pMtlLib = std::move(mtlLib);
  return 0;
}

#define EXIT_ERROR(TEXT) {\
  printf("%s:%zu: error: unknown command \"%s\"\n", absPathPtr, lineNum, (TEXT));\
  exit(EXIT_FAILURE);\
}

enum {
  VDATA_UNDEF = -1,
  VDATA_POS = 0,
  VDATA_NORM = 1,
  VDATA_TOTAL
};

int DetectVData(const char* token)
{
  if (strcmp(token, "v") == 0)
  {
    return VDATA_POS;
  }
  if (strcmp(token, "vn") == 0)
  {
    return VDATA_NORM;
  }
  return VDATA_UNDEF;
}

int main(int argc, char* argv[])
{
  if (bb::ProcessStartupArguments(argc, argv) != 0)
  {
    return -1;
  }

  if (argc != 2)
  {
    fprintf(stderr, "%s\n", "Error: No obj provided!");
    fprintf(stderr, "%s\n", "Usage: obj2mesh OBJ");
    return -1;
  }

  char* absPathPtr = realpath(argv[1], nullptr);
  if (absPathPtr == nullptr)
  {
    fprintf(stderr, "%s\n", "Error: Can't get absolute path for obj");
    return -1;
  }
  BB_DEFER(free(absPathPtr));

  FILE* input = fopen(absPathPtr, "rt");
  if (input == NULL)
  {
    fprintf(stderr, "File \"%s\" not found. ABORT.", absPathPtr);
    return -1;
  }
  BB_DEFER(fclose(input));

  char* text = nullptr;
  size_t len = 0;
  ssize_t nread;

  BB_DEFER(free(text));

  mtlLib_t mtlLib;
  vPos_t vPos[VDATA_TOTAL];

  bool hasColor = false;
  glm::vec3 color;

  size_t lineNum = 1;

  vPos_t triPos;
  vPos_t triNorm;
  vPos_t triCol;
  vInd_t triInd;
  uint16_t triCurIndex = 0;

  while ((nread = getline(&text, &len, input)) != -1)
  {
    if (text[0] == '\n')
    {
      ++lineNum;
      continue;
    }

    text = strtok(text, "\n");
    if (text == nullptr)
    {
      EXIT_ERROR("Invalid String");
    }

    switch (text[0])
    {
    case '#':
    case '\n':
      // skip comments and newlines
      break;
    case 'f':
      {
        char* token;
        token = strtok(text, " \n");
        if (strcmp(token, "f") != 0)
        {
          EXIT_ERROR(token);
        }

        for(char* face = strtok(nullptr, " \n"); face != nullptr; face = strtok(nullptr, " \n"))
        {
          char* cursor;
          errno = 0;
          auto vPosIndex = strtol(face, &cursor, 10);
          if ((face == cursor) || (errno != 0))
          {
            EXIT_ERROR("Invalid face format! There is no vertex data");
          }
          if (vPosIndex <= 0)
          {
            EXIT_ERROR("Invalid face format! There can't be no 0 index");
          }
          if (*cursor != '/')
          {
            if (vPos[VDATA_POS].size() <= static_cast<size_t>(vPosIndex))
            {
              EXIT_ERROR("Invalid face format! There is no such vertex index!");
            }
            triPos.emplace_back(
              vPos[VDATA_POS][static_cast<size_t>(vPosIndex-1)]
            );
            if (hasColor)
            {
              triCol.emplace_back(
                color
              );
            }
            triInd.emplace_back(
              triCurIndex++
            );
            continue;
          }

          face = cursor + 1;
          if (*face != '/')
          {
            EXIT_ERROR("Texture coordinates unsupported for now!");
          }
          ++face;

          errno = 0;
          auto vNormIndex = strtol(face, &cursor, 10);
          if (*cursor != 0)
          {
            EXIT_ERROR("Invalid face format! There is only pos/tex/normal available");
          }
          if (vNormIndex <= 0)
          {
            EXIT_ERROR("Invalid face format! There can't be no 0 index");
          }

          if (vPos[VDATA_POS].size() < static_cast<size_t>(vPosIndex))
          {
            EXIT_ERROR("Invalid face format! There is no such vertex pos index!");
          }
          if (vPos[VDATA_NORM].size() < static_cast<size_t>(vNormIndex))
          {
            EXIT_ERROR("Invalid face format! There is no such vertex normal index!");
          }

          triPos.emplace_back(
            vPos[VDATA_POS][static_cast<size_t>(vPosIndex-1)]
          );
          triNorm.emplace_back(
            vPos[VDATA_NORM][static_cast<size_t>(vNormIndex-1)]
          );
          if (hasColor)
          {
            triCol.emplace_back(
              color
            );
          }
          triInd.emplace_back(
            triCurIndex++
          );
        }
      }
      break;
    case 'm':
      {
        char* token;
        token = strtok(text, " \n");
        if (strcmp(token, "mtllib") != 0)
        {
          EXIT_ERROR(token);
        }

        char* fname = strtok(nullptr, " \n");
        if (fname == nullptr)
        {
          EXIT_ERROR("Invalid OBJ Format! MatLib filename not found");
        }
        
        if (LoadMaterials(fname, &mtlLib) != 0)
        {
          EXIT_ERROR("Invalid MTL Format!");
        }
      }
      break;
    case 'o':
      {
        char* token;
        token = strtok(text, " \n");
        if (strcmp(token, "o") != 0)
        {
          EXIT_ERROR(token);
        }

        char* objName = strtok(nullptr, " \n");
        if (objName == nullptr)
        {
          EXIT_ERROR("Invalid OBJ Format! Object name not found");
        }
        printf("Object: \"%s\"\n", objName);
      }
      break;
    case 's':
      {
        char* token;
        token = strtok(text, " \n");
        if (strcmp(token, "s") != 0)
        {
          EXIT_ERROR(token);
        }
        char* smoothMode = strtok(nullptr, " \n");
        printf("Smooth: \"%s\"\n", smoothMode);
      }
      break;
    case 'v':
      {
        char* token;
        token = strtok(text, " \n");

        int vDataType = DetectVData(token);
        if (vDataType == VDATA_UNDEF)
        {
          EXIT_ERROR(token);
        }

        glm::vec3 vPosValue;
        char* vData = strtok(nullptr, "\n");
        char* vNextData;

        errno = 0;
        vPosValue.x = strtof(vData, &vNextData);
        if ((errno != 0) || (vNextData == vData))
        {
          EXIT_ERROR("Bad Vertex Format X!");
        }
        vData = vNextData;

        errno = 0;
        vPosValue.y = strtof(vData, &vNextData);
        if ((errno != 0) || (vNextData == vData))
        {
          EXIT_ERROR("Bad Vertex Format Y!");
        }
        vData = vNextData;

        errno = 0;
        vPosValue.z = strtof(vData, &vNextData);
        if ((errno != 0) || (vNextData == vData))
        {
          EXIT_ERROR("Bad Vertex Format Z!");
        }
        vData = vNextData;

        vPos[vDataType].emplace_back(vPosValue);
      }
      break;
    case 'u':
      {
        char* token;
        token = strtok(text, " \n");
        if (strcmp(token, "usemtl") != 0)
        {
          EXIT_ERROR(token);
        }

        char* mtlName = strtok(nullptr, " \n");
        if (mtlName == nullptr)
        {
          EXIT_ERROR("Invalid OBJ Format! Material name is not found");
        }

        auto matData = mtlLib.find(mtlName);
        if (matData == mtlLib.end())
        {
          EXIT_ERROR("Invalid OBJ Format! No such material in library");
        }
        color = matData->second;
        hasColor = true;
      }
      break;
    default:
      EXIT_ERROR(text);
      break;
    }
    ++lineNum;
  }

  bb::meshDesc_t meshDesc;

  if (triPos.empty())
  {
    EXIT_ERROR("No Vertex Data Found!");
  }

  auto maxBox = MaxBox(triPos);
  auto minBox = MinBox(triPos);

  auto cenBox = (minBox + maxBox)/2.0f;

  meshDesc.Buffers().emplace_back(
    bb::MakeVertexBuffer(std::move(triPos))
  );
  if (!triCol.empty())
  {
    meshDesc.Buffers().emplace_back(
      bb::MakeVertexBuffer(std::move(triCol))
    );
  }
  if (!triNorm.empty())
  {
    meshDesc.Buffers().emplace_back(
      bb::MakeVertexBuffer(std::move(triNorm))
    );
  }
  meshDesc.Indecies() = bb::MakeIndexBuffer(std::move(triInd));
  meshDesc.SetDrawMode(GL_TRIANGLES);

  auto& context = bb::context_t::Instance();

  if (FILE* meshFile = fopen((std::string(absPathPtr) + ".msh").c_str(), "wb"))
  {
    BB_DEFER(fclose(meshFile));
    meshDesc.Save(meshFile);
  }

  auto mesh = bb::GenerateMesh(meshDesc);
  auto renderProgram = bb::shader_t::LoadProgramFromFiles("obj2mesh.vp.glsl", "obj2mesh.fp.glsl");
  auto worldCamera = bb::camera_t::Perspective(
    45.0f,
    context.AspectRatio(),
    0.1f,
    100.0f
  );
  worldCamera.View() = glm::lookAt(cenBox + glm::vec3(2.0f, 2.0f, 2.0f), cenBox, glm::vec3(0.0f, 1.0f, 0.0f));
  worldCamera.Update();

  BB_DEFER(
    mesh = bb::mesh_t();
    renderProgram = bb::shader_t();
    worldCamera = bb::camera_t();
  );

  auto lastTick = glfwGetTime();
  auto lastCursorPos = glm::vec2(context.MousePos());

  while(context.Update())
  {
    auto nowTick = glfwGetTime();
    auto delta = static_cast<float>(nowTick - lastTick);

    auto newCursorPos = glm::vec2(context.MousePos());
    if (context.IsButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
    {
      auto deltaCursorPos = (newCursorPos - lastCursorPos)*delta*0.25f;

      context.RelativeCursor(true);

      worldCamera.View() = 
        glm::scale(
          glm::rotate(
            worldCamera.View(),
            deltaCursorPos.x,
            glm::vec3(0.0f, 1.0f, 0.0f)
          ),
          glm::vec3(1.0f - deltaCursorPos.y)
        );
    }
    else
    {
      context.RelativeCursor(false);
    }
    lastCursorPos = newCursorPos;

    bb::framebuffer_t::Bind(context.Canvas());
    bb::shader_t::Bind(renderProgram);
    worldCamera.Update();
    renderProgram.SetBlock("camera", worldCamera.UniformBlock());

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    mesh.Render();

    lastTick = nowTick;
  }

  return 0;
}