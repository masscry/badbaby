#pragma once
#ifndef __BB_UTIL_SHAPES_VECFONT_HEADER__
#define __BB_UTIL_SHAPES_VECFONT_HEADER__

#include <algebra.hpp>

#include <map>
#include <cwchar>

namespace bb
{

  /**
   * Get vector font symbol.
   * 
   * @param smb symbol to get
   * @param pSize total number of points in symbol
   * 
   * @return pointer to symbol vertecies, or nullptr on invalid arguments
   */
  const glm::vec2* VectorFontSymbol(wint_t smb, size_t* pSize);

} // namespace bb

#endif /* __BB_UTIL_SHAPES_VECFONT_HEADER__ */