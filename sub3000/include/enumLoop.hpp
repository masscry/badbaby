/**
 * @file enumLoop.hpp
 * 
 * Automatic enumeration loop support.
 */

#pragma once
#ifndef __SUB3000_ENUM_LOOP_HEADER__
#define __SUB3000_ENUM_LOOP_HEADER__

namespace sub3000
{

  template<typename enum_t>
  class enumLoop_t final
  {
  public:
    class iterator_t final
    {
      int curVal;
    public:

      iterator_t(const iterator_t&) = default;
      iterator_t(iterator_t&&) = default;

      iterator_t& operator= (const iterator_t&) = default;
      iterator_t& operator= (iterator_t&&) = default;

      iterator_t(int curVal)
      : curVal(curVal)
      {
        ;
      }

      enum_t operator*() const
      {
        return static_cast<enum_t>(this->curVal);
      }

      void operator++( void )
      {
        ++this->curVal;
      }

      bool operator!=(enumLoop_t<enum_t>::iterator_t rhs)
      {
        return this->curVal != rhs.curVal;
      }
    };
  };


  template<typename enum_t>
  typename enumLoop_t<enum_t>::iterator_t begin(enumLoop_t<enum_t>)
  {
    return typename enumLoop_t<enum_t>::iterator_t(static_cast<int>(enum_t::firstItem));
  }

  template<typename enum_t>
  typename enumLoop_t<enum_t>::iterator_t end(enumLoop_t<enum_t>)
  {
    return typename enumLoop_t<enum_t>::iterator_t(static_cast<int>(enum_t::totalItems));
  }
  
} // namespace sub3000

#endif /* __SUB3000_ENUM_LOOP_HEADER__ */