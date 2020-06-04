/**
 * @file print.hpp
 * 
 * Simple wrapper for short string formatting in Pascal/Python style
 */

#pragma once
#ifndef __BB_PRINT_FUNCTION_HEADER__
#define __BB_PRINT_FUNCTION_HEADER__

#include <string>
#include <sstream>

namespace bb
{
  class Print
  {
    std::string str;

    template<typename... args_t>
    void Prepare(std::stringstream& ss, args_t&& ... args);

    template<typename head_t>
    void Prepare(std::stringstream& ss, head_t&& head)
    {
      ss << std::forward<head_t>(head);
    }

    template<typename head_t, typename... tail_t>
    void Prepare(std::stringstream& ss, head_t&& head, tail_t&&... tail)
    {
      ss << std::forward<head_t>(head);
      this->Prepare(ss, std::forward<tail_t>(tail)...);
    }

  public:

    operator std::string() const
    {
      return this->str;
    }

    Print()
    {
      ;
    }

    template<typename... args_t>
    Print(args_t&&... args)
    {
      std::stringstream ss;
      this->Prepare(ss, std::forward<args_t>(args)...);
      this->str = ss.str();
    }
    
    Print(const Print&) = default;
    Print(Print&&) = default;
    Print& operator=(const Print&) = default;
    Print& operator=(Print&&) = default;
    ~Print() = default;
  };

} // namespace bb

#endif /* __BB_PRINT_FUNCTION_HEADER__ */