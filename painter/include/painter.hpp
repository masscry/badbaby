/**
 * @file painter.hpp
 */

#pragma once
#ifndef __BB_PAINTER_HEADER__
#define __BB_PAINTER_HEADER__

#include <msg.hpp>

namespace paint
{

  class update_t final: public bb::msg::basic_t
  {
  public:
    update_t() { ; }
    ~update_t() override = default;
  };

  class exit_t final: public bb::msg::basic_t
  {
  public:
    exit_t() { ; }
    ~exit_t() override = default;
  };

  /**
   * Post message to main update loop.
   *
   * @param msg message to main
   */
  void PostToMain(bb::msg_t&& msg);

  const char* MainMsgToStr(const bb::msg_t& msg);

}

#endif /* __BB_PAINTER_HEADER__ */
