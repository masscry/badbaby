/**
 * @file value.hpp
 * 
 * Simple abstract value wrapper.
 * 
 */

#pragma once
#ifndef __BB_COMMON_CONFIG_VALUE_HEADER__
#define __BB_COMMON_CONFIG_VALUE_HEADER__

#include <unordered_map>
#include <memory>
#include <string>

namespace bb
{

  enum class type_t
  {
    none,
    number,
    string
  };

  class value_t
  {

    value_t(const value_t&) = delete;
    value_t& operator =(const value_t&) = delete;
    value_t(value_t&&) = delete;
    value_t& operator =(value_t&&) = delete;

  public:

    virtual bool operator == (const value_t& rhs) = 0;
    virtual std::string ToString() const = 0;
    virtual type_t Type() const = 0;
    virtual double Number() const = 0;
    virtual const std::string& String() const = 0;

    value_t();
    virtual ~value_t() = 0;

  };

  class ref_t
  {
    std::unique_ptr<value_t> val;

    ref_t(const ref_t&) = delete;
    ref_t& operator=(const ref_t&) = delete;

    ref_t(std::unique_ptr<value_t>&& val);

  public:

    type_t Type() const
    {
      if (this->val)
      {
        return this->val->Type();
      }
      return type_t::none;
    }

    std::string ToString() const
    {
      if (this->val)
      {
        return this->val->ToString();
      }
      return std::string("none");
    }

    double Number() const
    {
      return this->val->Number();
    }

    const std::string& String() const
    {
      return this->val->String();
    }

    ref_t()
    {
      ;
    }

    ~ref_t()
    {
      ;
    }

    ref_t(ref_t&& move) noexcept
    :val(std::move(move.val))
    {
      ;
    }

    bool operator == (const ref_t& ref)
    {
      if ((this->Type() == type_t::none) || (ref.Type() == type_t::none))
      {
        return false;
      }
      return this->val->operator==(*ref.val);
    }

    bool operator != (const ref_t& ref)
    {
      return !this->operator==(ref);
    }

    ref_t& operator=(ref_t&& move) noexcept
    {
      if (this == &move)
      {
        return *this;
      }
      this->val = std::move(move.val);
      return *this;
    }

    static ref_t Number(double value);
    static ref_t String(const std::string& value);

  };

} // namespace bb

#endif /* __BB_COMMON_CONFIG_HEADER__ */