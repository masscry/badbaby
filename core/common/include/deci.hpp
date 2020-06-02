/**
 * @file deci.hpp
 * 
 * Decimal number representation
 * 
 */

#pragma once
#ifndef __BAD_BABY_DECI_HEADER__
#define __BAD_BABY_DECI_HEADER__

#include <cstdint>
#include <cmath>
#include <iostream>
#include <complex>

namespace bb
{

  class deci_t final
  {
  public:
    enum
    {
      fraction = 1000
    };
  
  private:

    int32_t value;

  public:

    explicit operator double() const
    {
      int32_t ival = this->value / fraction;
      int32_t fval = this->value % fraction;
      return static_cast<double>(ival) + static_cast<double>(fval) * 0.001;
    }

    int32_t Integer() const
    {
      return this->value / fraction;
    }

    int32_t Fraction() const
    {
      return this->value % fraction;
    }

    deci_t& operator+=(const deci_t& rhs)
    {
      this->value += rhs.value;
      return *this;
    }

    deci_t& operator-=(const deci_t& rhs)
    {
      this->value -= rhs.value;
      return *this;
    }

    deci_t& operator*=(const deci_t& rhs)
    {
      int64_t newVal = static_cast<int64_t>(this->value) * rhs.value / fraction;
      this->value = static_cast<int32_t>(newVal & 0xFFFFFFFF);
      return *this;
    }

    deci_t& operator/=(const deci_t& rhs)
    {
      int64_t newVal = static_cast<int64_t>(this->value) * fraction / rhs.value;
      this->value = static_cast<int32_t>(newVal & 0xFFFFFFFF);
      return *this;
    }

    deci_t& operator%=(const deci_t& rhs)
    {
      this->value %= rhs.value;
      return *this;
    }

    friend deci_t operator % (deci_t lhs, const deci_t& rhs)
    {
      lhs %= rhs;
      return lhs;
    }

    friend deci_t operator+(deci_t lhs, const deci_t& rhs)
    {
      lhs += rhs;
      return lhs;
    }

    friend deci_t operator-(deci_t lhs, const deci_t& rhs)
    {
      lhs -= rhs;
      return lhs;
    }

    friend deci_t operator*(deci_t lhs, const deci_t& rhs)
    {
      lhs *= rhs;
      return lhs;
    }

    friend deci_t operator/(deci_t lhs, const deci_t& rhs)
    {
      lhs -= rhs;
      return lhs;
    }

    friend bool operator< (const deci_t& lhs, const deci_t& rhs)
    {
      return lhs.value < rhs.value; 
    }

    friend bool operator> (const deci_t& lhs, const deci_t& rhs)
    {
      return rhs < lhs; 
    }

    friend bool operator<=(const deci_t& lhs, const deci_t& rhs)
    {
      return !(lhs > rhs);
    }

    friend bool operator>=(const deci_t& lhs, const deci_t& rhs)
    {
      return !(lhs < rhs); 
    }

    friend bool operator==(const deci_t& lhs, const deci_t& rhs)
    {
      return lhs.value == rhs.value;
    }

    friend bool operator!=(const deci_t& lhs, const deci_t& rhs)
    {
      return !(lhs == rhs); 
    }

    deci_t()
    : value(0)
    {
      ;
    }

    deci_t(double value)
    : value(static_cast<int32_t>(round(value*deci_t::fraction)))
    {
      ;
    }

    deci_t(int32_t value)
    : value(value*deci_t::fraction)
    {
      ;
    }

    deci_t(int32_t ival, int32_t fval)
    : value(ival*deci_t::fraction + fval)
    {
      ;
    }

    deci_t& operator=(const deci_t& deci) = default;
    deci_t& operator=(deci_t&& deci) noexcept = default;
    deci_t(const deci_t& deci) = default;
    deci_t(deci_t&& deci) noexcept = default;
    ~deci_t() = default;
  };

  std::ostream& operator<<(std::ostream& os, const deci_t& obj);
  std::istream& operator>>(std::istream& is, deci_t& obj);

} // namespace bb

#endif /* __BAD_BABY_DECI_HEADER__ */