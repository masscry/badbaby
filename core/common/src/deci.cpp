#include <deci.hpp>

namespace bb
{

  std::ostream& operator<<(std::ostream& os, const deci_t& obj)
  {
    return os << obj.Integer() << '.' << obj.Fraction();
  }
  
  std::istream& operator>>(std::istream& is, deci_t& obj)
  {
    int32_t iVal;
    int32_t fVal;
    char dotChr = '\0';

    if (is >> iVal)
    {
      is >> dotChr;
      if (dotChr == '.')
      {
        if (is >> fVal)
        {
          while (fVal > deci_t::fraction)
          {
            fVal /= 10;
          }
          obj = deci_t(iVal, fVal);
        }
        else
        {
          obj = deci_t(iVal);
        }
      }
      else
      {
        if (is)
        {
          is.putback(dotChr);
        }
        obj = deci_t(iVal);
      }
    }
    else
    {
      is.setstate(std::ios_base::failbit);
    }
    return is;
  }
  
} // namespace bb
