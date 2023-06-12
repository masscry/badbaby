#include <deci.hpp>
#include <common.hpp>

#include <cassert>
#include <iostream>

int main(int, char* [])
{
  using namespace bb;

  auto a = deci_t(100);
  BB_ASSERT(a.Integer() == 100);
  BB_ASSERT(a.Fraction() == 0);

  a = deci_t(1.1);
  BB_ASSERT(a.Integer() == 1);
  BB_ASSERT(a.Fraction() == 100);

  auto b = deci_t(10);

  b *= a;
  BB_ASSERT(b.Integer() == 11);
  BB_ASSERT(b.Fraction() == 0);

  b /= 20;
  BB_ASSERT(b.Integer() == 0);
  BB_ASSERT(b.Fraction() == 550);

  return 0;
}