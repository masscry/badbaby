#include <shapes.hpp>
#include <common.hpp>
#include <context.hpp>
#include <utf8.hpp>
#include <vecfont.hpp>

#include <cstdio>
#include <cmath>
#include <utility>
#include <memory>
#include <limits>

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
            BB_PANIC();
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
  }

  meshDesc_t DefineNumber(glm::vec3 offset, float width, glm::vec2 scale, const char* utf8Text)
  {
    if (utf8Text == nullptr)
    {
      return meshDesc_t();
    }

    auto wideNumber = bb::utf8extract(utf8Text);

    meshDesc_t result;

    glm::vec2 cursor(0.0f);

    for(auto smb: wideNumber)
    {
      if (IsSpace(static_cast<wint_t>(smb)))
      {
        cursor.x += 1.0f;
        continue;
      }

      auto smbScale = scale;
      auto smbCursor = cursor;

      if (IsLower(static_cast<wint_t>(smb)))
      {
        smbCursor.y += 0.6f;
        smbScale.y *= 0.6f;
      }

      size_t smbSize;
      auto smbVerts = VectorFontSymbol(static_cast<wint_t>(smb), &smbSize);
      if (smbVerts == nullptr)
      { // This won't happen, if no regression happens
        bb::Error("Unknown symbol: %08x (%c)", smb, (smb & 0xFF));
        BB_PANIC();
        return meshDesc_t();
      }

      result.Append(
        AddSymbol(offset, width, smbScale, smbVerts, smbSize, smbCursor)
      );
      cursor.x += 1.0f;
    }
    return result;
  }

}
