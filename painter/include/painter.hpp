/**
 * @file painter.hpp
 */

#pragma once
#ifndef __BB_PAINTER_HEADER__
#define __BB_PAINTER_HEADER__

#include <msg.hpp>

namespace paint
{

  enum mainMessage_t: uint16_t
  {
    nop = bb::msgID_t::USR00,
    update,
    exit
  };

  /**
   * Post message to main update loop.
   *
   * @param msg message to main
   */
  void PostToMain(bb::msg_t msg);

  const char* MainMsgToStr(const bb::msg_t& msg);

}

#endif /* __BB_PAINTER_HEADER__ */
