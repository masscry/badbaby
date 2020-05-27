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

  const auto BreakPoint = glm::vec2(2.0f, 0.0f);

  const glm::vec2 tZero[] = {
    { 0.0f, 0.0f },
    { 0.5f, 0.0f },
    { 0.5f, 1.0f },
    { 0.0f, 1.0f },
    { 0.0f, 0.0f },
    BreakPoint,
    { 0.0f, 1.0f },
    { 0.5f, 0.0f },
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


  const glm::vec2 tMinus[] = {
    { 0.0f, 0.5f },
    { 0.5f, 0.5f },
  };

  const glm::vec2 tUnderscore[] = {
    { 0.0f, 0.75f },
    { 0.0f, 1.0f },
    { 0.5f, 1.0f },
    { 0.5f, 0.75f },
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

  const glm::vec2 tLetA[] = {
    { 0.00f, 1.00f },
    { 0.25f, 0.00f },
    { 0.50f, 1.00f },
    BreakPoint,
    { 0.05f, 0.75f },
    { 0.45f, 0.75f },
  };

  const glm::vec2 tLetB[] = {
    { 0.00f, 1.00f },
    { 0.00f, 0.00f },
    { 0.40f, 0.00f },
    { 0.40f, 0.50f },
    { 0.00f, 0.50f },
    { 0.50f, 0.50f },
    { 0.50f, 1.00f },
    { 0.00f, 1.00f }
  };

  const glm::vec2 tLetC[] = {
    { 0.50f, 0.30f },
    { 0.50f, 0.00f },
    { 0.00f, 0.00f },
    { 0.00f, 1.00f },
    { 0.50f, 1.00f },
    { 0.50f, 0.70f },
  };

  const glm::vec2 tLetD[] = {
    { 0.00f, 0.00f },
    { 0.00f, 1.00f },
    { 0.40f, 1.00f },
    { 0.50f, 0.70f },
    { 0.50f, 0.30f },
    { 0.40f, 0.00f },
    { 0.00f, 0.00f },
  };

  const glm::vec2 tLetE[] = {
    { 0.50f, 0.00f },
    { 0.00f, 0.00f },
    { 0.00f, 1.00f },
    { 0.50f, 1.00f },
    BreakPoint,
    { 0.00f, 0.50f },
    { 0.40f, 0.50f },
  };

  const glm::vec2 tLetF[] = {
    { 0.50f, 0.00f },
    { 0.00f, 0.00f },
    { 0.00f, 1.00f },
    BreakPoint,
    { 0.00f, 0.50f },
    { 0.40f, 0.50f },
  };

  const glm::vec2 tLetG[] = {
    { 0.50f, 0.00f },
    { 0.00f, 0.00f },
    { 0.00f, 1.00f },
    { 0.50f, 1.00f },
    { 0.50f, 0.50f },
    { 0.30f, 0.50f }
  };

  const glm::vec2 tLetH[] = {
    { 0.00f, 1.00f },
    { 0.00f, 0.00f },
    BreakPoint,
    { 0.50f, 1.00f },
    { 0.50f, 0.00f },
    BreakPoint,
    { 0.00f, 0.50f },
    { 0.50f, 0.50f },
  };

  const glm::vec2 tLetI[] = {
    { 0.40f, 0.00f },
    { 0.10f, 0.00f },
    BreakPoint,
    { 0.40f, 1.00f },
    { 0.10f, 1.00f },
    BreakPoint,
    { 0.25f, 0.00f },
    { 0.25f, 1.00f },
  };

  const glm::vec2 tLetJ[] = {
    { 0.10f, 0.00f },
    { 0.40f, 0.00f },
    { 0.40f, 1.00f },
    { 0.10f, 1.00f },
    { 0.10f, 0.70f }
  };

  const glm::vec2 tLetK[] = {
    { 0.00f, 1.00f },
    { 0.00f, 0.00f },
    BreakPoint,
    { 0.40f, 0.00f },
    { 0.00f, 0.40f },
    { 0.50f, 1.00f },
  };

  const glm::vec2 tLetL[] = {
    { 0.00f, 0.00f },
    { 0.00f, 1.00f },
    { 0.50f, 1.00f }
  };

  const glm::vec2 tLetM[] = {
    { 0.00f, 1.00f },
    { 0.00f, 0.00f },
    { 0.25f, 0.40f },
    { 0.50f, 0.00f },
    { 0.50f, 1.00f }
  };

  const glm::vec2 tLetN[] = {
    { 0.00f, 1.00f },
    { 0.00f, 0.00f },
    { 0.50f, 1.00f },
    { 0.50f, 0.00f }
  };

  const glm::vec2 tLetO[] = {
    { 0.0f, 0.0f },
    { 0.5f, 0.0f },
    { 0.5f, 1.0f },
    { 0.0f, 1.0f },
    { 0.0f, 0.0f }
  };

  const glm::vec2 tLetP[] = {
    { 0.0f, 1.0f },
    { 0.0f, 0.0f },
    { 0.5f, 0.0f },
    { 0.5f, 0.5f },
    { 0.0f, 0.5f }
  };

  const glm::vec2 tLetQ[] = {
    { 0.0f, 0.0f },
    { 0.5f, 0.0f },
    { 0.5f, 0.7f },
    { 0.0f, 1.0f },
    { 0.0f, 0.0f },
    BreakPoint,
    { 0.5f, 1.0f },
    { 0.2f, 0.6f }
  };


  const glm::vec2 tLetR[] = {
    { 0.0f, 1.0f },
    { 0.0f, 0.0f },
    { 0.4f, 0.0f },
    { 0.4f, 0.5f },
    { 0.0f, 0.5f },
    BreakPoint,
    { 0.0f, 0.5f },
    { 0.5f, 1.0f }
  };


  const glm::vec2 tLetS[] = {
    { 0.5f,  0.25f },
    { 0.5f,  0.00f },
    { 0.0f,  0.00f },
    { 0.0f,  0.50f },
    { 0.5f,  0.50f },
    { 0.5f,  1.00f },
    { 0.0f,  1.00f },
    { 0.0f,  0.75f },
  };

  const glm::vec2 tLetT[] = {
    { 0.25f, 1.00f },
    { 0.25f, 0.00f },
    BreakPoint,
    { 0.00f, 0.00f },
    { 0.50f, 0.00f },
  };

  const glm::vec2 tLetU[] = {
    { 0.00f, 0.00f },
    { 0.00f, 0.80f },
    { 0.25f, 1.00f },
    { 0.50f, 0.80f },
    { 0.50f, 0.00f }
  };

  const glm::vec2 tLetV[] = {
    { 0.00f, 0.00f },
    { 0.25f, 1.00f },
    { 0.50f, 0.00f }
  };

  const glm::vec2 tLetW[] = {
    { 0.00f, 0.00f },
    { 0.15f, 1.00f },
    { 0.25f, 0.70f },
    { 0.35f, 1.00f },
    { 0.50f, 0.00f }
  };

  const glm::vec2 tLetX[] = {
    { 0.00f, 0.00f },
    { 0.50f, 1.00f },
    BreakPoint,
    { 0.00f, 1.00f },
    { 0.50f, 0.00f }
  };

  const glm::vec2 tLetY[] = {
    { 0.00f, 0.00f },
    { 0.25f, 0.50f },
    { 0.50f, 0.00f },
    BreakPoint,
    { 0.25f, 0.50f },
    { 0.25f, 1.00f },
  };

  const glm::vec2 tLetZ[] = {
    { 0.00f, 0.00f },
    { 0.50f, 0.00f },
    { 0.00f, 1.00f },
    { 0.50f, 1.00f },
  };

  const glm::vec2* tLetter[] = {
    tLetA, // a 
    tLetB, // b
    tLetC, // c
    tLetD, // d
    tLetE, // e
    tLetF, // f
    tLetG, // g
    tLetH, // h
    tLetI, // i
    tLetJ, // j
    tLetK, // k
    tLetL, // l
    tLetM, // m
    tLetN, // n
    tLetO, // o
    tLetP, // p
    tLetQ, // q
    tLetR, // r
    tLetS, // s
    tLetT, // t
    tLetU, // u
    tLetV, // v
    tLetW, // w
    tLetX, // x
    tLetY, // y
    tLetZ  // z
  };

  const size_t tMinusSize = bb::countof(tMinus);

  const size_t tUnderscoreSize = bb::countof(tUnderscore);

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

  const size_t tLetterSize[] = {
    bb::countof(tLetA), // a 
    bb::countof(tLetB), // b
    bb::countof(tLetC), // c
    bb::countof(tLetD), // d
    bb::countof(tLetE), // e
    bb::countof(tLetF), // f
    bb::countof(tLetG), // g
    bb::countof(tLetH), // h
    bb::countof(tLetI), // i
    bb::countof(tLetJ), // j
    bb::countof(tLetK), // k
    bb::countof(tLetL), // l
    bb::countof(tLetM), // m
    bb::countof(tLetN), // n
    bb::countof(tLetO), // o
    bb::countof(tLetP), // p
    bb::countof(tLetQ), // q
    bb::countof(tLetR), // r
    bb::countof(tLetS), // s
    bb::countof(tLetT), // t
    bb::countof(tLetU), // u
    bb::countof(tLetV), // v
    bb::countof(tLetW), // w
    bb::countof(tLetX), // x
    bb::countof(tLetY), // y
    bb::countof(tLetZ)  // z
  };

  static_assert(bb::countof(tNumber) == bb::countof(tNumberSize),
    "tNumber arrays size must be equal to tNumberSize array size"
  );

  static_assert(bb::countof(tLetter) == bb::countof(tLetterSize),
    "tLetter arrays size must be equal to tLetterSize array size"
  );

}


namespace bb
{

  namespace
  {
    meshDesc_t AddSymbol(glm::vec3 offset, float width,  glm::vec2 scale, const glm::vec2* points, size_t totalPoints, glm::vec2 cursor)
    {
      linePoints_t linePoints;
      meshDesc_t result;

      for (size_t i = 0; i < totalPoints; ++i)
      {
        if (points[i].x > 1.0f)
        { // special case! breaking vertex starts new line
          if (linePoints.size() < 2)
          { // if not enought points, just skip - may be error in data
            assert(0);
            continue;
          }
          if (result.IsGood())
          {
            result.Append(DefineLine(offset, width, linePoints));
          }
          else
          {
            result = DefineLine(offset, width, linePoints);
          }
          linePoints.clear();
          continue;
        }

        linePoints.emplace_back(
          (points[i]+cursor)*scale
        );
      }

      if (!linePoints.empty())
      {
        if (result.IsGood())
        {
          result.Append(DefineLine(offset, width, linePoints));
        }
        else
        {
          result = DefineLine(offset, width, linePoints);
        }
      }
      return result;
    }

    meshDesc_t AddDigit(glm::vec3 offset, float width,  glm::vec2 scale, char number, glm::vec2 cursor)
    {
      assert(std::isdigit(number));

      auto index = number - '0';
      return AddSymbol(offset, width, scale, tNumber[index], tNumberSize[index], cursor);
    }

    meshDesc_t AddLetter(glm::vec3 offset, float width,  glm::vec2 scale, char number, glm::vec2 cursor)
    {
      assert(std::isalpha(number));

      int index = -1;
      if (std::islower(number))
      {
        index = number - 'a';
        scale.y *= 0.60f;
        cursor.y += 0.6f;
      }

      if (std::isupper(number))
      {
        index = number - 'A';
      }

      if ((index < 0) || (static_cast<size_t>(index) >= bb::countof(tLetter)))
      {
        assert(0);
        return AddSymbol(offset, width, scale, tUnderscore, tUnderscoreSize, cursor);
      }

      return AddSymbol(offset, width, scale, tLetter[index], tLetterSize[index], cursor);
    }

  }

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
      if (std::isdigit(*number))
      {
        result.Append(
          AddDigit(offset, width, scale, *number, cursor)
        );
        ++number;
        cursor.x += 1.0f;
        continue;
      }

      if (std::isspace(*number))
      {
        ++number;
        cursor.x += 1.0f;
        continue;
      }

      if (std::isalpha(*number))
      {
        result.Append(
          AddLetter(offset, width, scale, *number, cursor)
        );
        ++number;
        cursor.x += 1.0f;
        continue;
      }

      if (std::isgraph(*number))
      {
        switch(*number)
        {
          case '-':
            result.Append(
              AddSymbol(offset, width, scale, tMinus, tMinusSize, cursor)
            );
            ++number;
            cursor.x += 1.0f;
            continue;
          default:
            result.Append(
              AddSymbol(offset, width, scale, tUnderscore, tUnderscoreSize, cursor)
            );
            ++number;
            cursor.x += 1.0f;
            continue;
        }
      }

      bb::Error("Unknown symbol: %c", *number);
      assert(0);
      return meshDesc_t();
    }
    return result;
  }

}
