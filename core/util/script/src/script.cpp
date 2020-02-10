#include <script.hpp>

#include <cctype>
#include <cassert>

namespace bb
{

  int vm_t::Push(ref_t&& ref)
  {
    this->listOfRefs.emplace_back(std::move(ref));
    return 0;
  }

  int vm_t::Command(int cmd)
  {
    int result = this->OnCommand(cmd, this->listOfRefs);
    this->listOfRefs.clear();
    return result;
  }

  vm_t::vm_t()
  {
    ;
  }

  vm_t::~vm_t()
  {
    ;
  }

  vm_t::vm_t(vm_t&& vm)
  : listOfRefs(std::move(vm.listOfRefs))
  {
    ;
  }

  vm_t& vm_t::operator = (vm_t&& vm)
  {
    if (this != &vm)
    {
      this->listOfRefs = std::move(vm.listOfRefs);
    }
    return *this;
  }

  enum scriptMode_t
  {
    SM_COMMAND = 0,
    SM_ARG,
    SM_NEXT_OR_END
  };

  int ExecuteScript(vm_t& vm, const char* script)
  {
    const char* cursor = script;
    scriptMode_t scriptMode = SM_COMMAND;

    int cmd = 0;
    while(*cursor != 0)
    {
      switch(scriptMode)
      {
      case SM_COMMAND:
        if (std::isalpha(*cursor) != 0)
        {
          cmd = *cursor;
          ++cursor;
          scriptMode = SM_NEXT_OR_END;
        }
        else
        {
          return -1;
        }
        break;
      case SM_ARG:
        {
          char* afterArg = 0;
          double arg = std::strtod(cursor, &afterArg);
          if (errno == ERANGE)
          {
            errno = 0;
            return -1;
          }
          if (afterArg == cursor)
          {
            return -1;
          }
          if (vm.Push(ref_t::Number(arg)) != 0)
          {
            return -1;
          }
          cursor = afterArg;
          scriptMode = SM_NEXT_OR_END;
        }
        break;
      case SM_NEXT_OR_END:
        switch(*cursor)
        {
          case ':':
            ++cursor;
            scriptMode = SM_ARG;
            break;
          case ';':
            if (cmd != 0)
            {
              if (vm.Command(cmd) != 0)
              {
                return -1;
              }
            }
            cmd = 0;
            ++cursor;
            while(std::isspace(*cursor) != 0)
            {
              ++cursor;
            }
            scriptMode = SM_COMMAND;
            break;
          default:
            return -1;
        }
        break;
      default:
        assert(0);
        return -1;
      }
    }
    return 0;
  }

} // namespace bb
