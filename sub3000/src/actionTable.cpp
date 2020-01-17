#include <actionTable.hpp>

#include <stdexcept>
#include <tuple>

namespace sub3000
{

  using gameActionListItem_t = std::tuple<const char*, gameAction_t>;

  gameActionListItem_t gameActionList[] = {
    { "action.game.new", sub3000::gameAction_t::newGame  },
    { "action.logo",     sub3000::gameAction_t::logo     },
    { "action.authors",  sub3000::gameAction_t::authors  },
    { "action.exit",     sub3000::gameAction_t::exit     },
    { "action.settings", sub3000::gameAction_t::settings }
  };

  const char* GetTextForAction(gameAction_t action)
  {
    for(const auto& item: gameActionList)
    {
      if (action == std::get<1>(item))
      {
        return std::get<0>(item);
      }
    }
    return "???";
  }

  gameAction_t GetActionFromText(const std::string& actionText)
  {
    for(const auto& item: gameActionList)
    {
      if (actionText.compare(std::get<0>(item)) == 0)
      {
        return std::get<1>(item);
      }
    }
    throw std::runtime_error(std::string("Unknown action: ") + actionText);
  }

} // namespace sub3000
