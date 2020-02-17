#include <shapes.hpp>
#include <common.hpp>
#include <context.hpp>

#include <cstdio>
#include <cmath>
#include <utility>
#include <memory>
#include <limits>

#include <glm/glm.hpp>

namespace
{

  const glm::vec2 tZero[] = {
    { 0.0f, 0.0f },
    { 0.5f, 0.0f },
    { 0.5f, 1.0f },
    { 0.0f, 1.0f },
    { 0.0f, 0.0f }
  };

  const glm::vec2 tOne[] = {
    { 0.0f, 0.5f },
    { 0.5f, 0.0f },
    { 0.5f, 1.0f }
  };

  const glm::vec2 tTwo[] = {
    { 0.0f, 0.0f },
    { 0.5f, 0.0f },
    { 0.5f, 0.5f },
    { 0.0f, 1.0f },
    { 0.5f, 1.0f }
  };

  const glm::vec2 tThree[] = {
    { 0.0f, 0.0f },
    { 0.5f, 0.0f },
    { 0.0f, 0.5f },
    { 0.5f, 0.5f },
    { 0.0f, 1.0f }
  };

  const glm::vec2 tFour[] = {
    { 0.5f, 1.0f },
    { 0.5f, 0.0f },
    { 0.0f, 0.5f },
    { 0.5f, 0.5f }
  };

  const glm::vec2 tFive[] = {
    { 0.5f, 0.0f },
    { 0.0f, 0.0f },
    { 0.0f, 0.5f },
    { 0.5f, 0.5f },
    { 0.5f, 1.0f },
    { 0.0f, 1.0f }
  };

  const glm::vec2 tSix[] = {
    { 0.5f, 0.0f },
    { 0.0f, 0.5f },
    { 0.0f, 1.0f },
    { 0.5f, 1.0f },
    { 0.5f, 0.5f },
    { 0.0f, 0.5f }
  };

  const glm::vec2 tSeven[] = {
    { 0.0f, 0.0f },
    { 0.5f, 0.0f },
    { 0.0f, 0.5f },
    { 0.0f, 1.0f }
  };

  const glm::vec2 tEight[] = {
    { 0.0f, 0.5f },
    { 0.0f, 1.0f },
    { 0.5f, 1.0f },
    { 0.5f, 0.0f },
    { 0.0f, 0.0f },
    { 0.0f, 0.5f },
    { 0.5f, 0.5f }
  };

  const glm::vec2 tNine[] = {
    { 0.5f, 0.5f },
    { 0.0f, 0.5f },
    { 0.0f, 0.0f },
    { 0.5f, 0.0f },
    { 0.5f, 0.5f },
    { 0.0f, 1.0f }
  };

  const glm::vec2* tNumber[] = {
    tZero,
    tOne,
    tTwo,
    tThree,
    tFour,
    tFive,
    tSix,
    tSeven,
    tEight,
    tNine
  };

  const size_t tNumberSize[] = {
    bb::countof(tZero),
    bb::countof(tOne),
    bb::countof(tTwo),
    bb::countof(tThree),
    bb::countof(tFour),
    bb::countof(tFive),
    bb::countof(tSix),
    bb::countof(tSeven),
    bb::countof(tEight),
    bb::countof(tNine)
  };

  static_assert(bb::countof(tNumber) == bb::countof(tNumberSize),
    "tNumber arrays size must be equal to tNumberSize array size"
  );

}


namespace bb
{

  meshDesc_t DefineNumber(glm::vec3 offset, float width, glm::vec2 scale, const char* number)
  {
    if (number == nullptr)
    {
      return meshDesc_t();
    }

    meshDesc_t result;

    glm::vec2 cursor(0.0f);

    while(*number != '\0')
    {
      if (!std::isdigit(*number))
      { // Only digits expected
        assert(0);
        return meshDesc_t();
      }

      auto index = *number - '0';

      auto points = tNumber[index];
      auto totalPoints = tNumberSize[index];

      linePoints_t linePoints;

      for (size_t i = 0; i < totalPoints; ++i)
      {
        linePoints.emplace_back(
          (points[i].x+cursor.x)*scale.x,
          (points[i].y+cursor.y)*scale.y
        );
      }
      result.Append(DefineLine(offset, width, linePoints));
      ++number;
      cursor.x += 1.0f;
    }
    return result;
  }

}
