#include <deci.hpp>

#include <cassert>
#include <iostream>

int main(int, char* [])
{
  using namespace bb;

  auto a = deci_t(100);
  assert(a.Integer() == 100);
  assert(a.Fraction() == 0);

  a = deci_t(1.1);
  assert(a.Integer() == 1);
  assert(a.Fraction() == 100);

  auto b = deci_t(10);

  b *= a;
  assert(b.Integer() == 11);
  assert(b.Fraction() == 0);

  b /= 20;
  assert(b.Integer() == 0);
  assert(b.Fraction() == 550);

  return 0;
}