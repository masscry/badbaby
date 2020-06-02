/**
 * @file mainMenu.h
 * @author main menu scene
 */

#pragma once
#ifndef __SUB3000_ACTION_TABLE_HEADER__
#define __SUB3000_ACTION_TABLE_HEADER__

#include <string>

namespace sub3000
{

  enum class gameAction_t: uint32_t
  {
    firstItem = 0,
    newGame = firstItem,
    logo,
    authors,
    exit,
    settings,
    demo,
    totalItems
  };

  gameAction_t GetActionFromText(const std::string& actionText);

  const char* GetTextForAction(gameAction_t action);


} // namespace sub3000

#endif /* __SUB3000_ACTION_TABLE_HEADER__ */