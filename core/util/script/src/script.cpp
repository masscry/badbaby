#include <script.hpp>
#include <common.hpp>

#include <cctype>
#include <cassert>
#include <cstdio>
#include <memory>

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

  double Argument(const listOfRefs_t& refs, uint32_t id)
  {
    if (id < refs.size())
    {
      auto item = refs.begin();
      std::advance(item, id);
      return item->Number();
    }
    return 0.0;
  }

  char* ReadWholeFile(FILE* input, size_t* pSize)
  {
    if (fseek(input, 0, SEEK_END) != 0)
    {
      return nullptr;
    }

    auto inputFileSize = ftell(input);
    if (inputFileSize < 0)
    {
      return nullptr;
    }

    if (fseek(input, 0, SEEK_SET) != 0)
    {
      return nullptr;
    }

    char* result = reinterpret_cast<char*>(malloc(inputFileSize+1));
    if (result == nullptr)
    {
      return nullptr;
    }

    if (fread(result, 1, inputFileSize, input) != static_cast<size_t>(inputFileSize))
    {
      free(result);
      return nullptr;
    }

    result[inputFileSize] = 0;
    if (pSize != nullptr)
    {
      *pSize = inputFileSize + 1;
    }
    return result;
  }

  char* ReadWholeFile(const char* filename, size_t* pSize)
  {
    FILE* input = fopen(filename, "rb");
    if (input == nullptr)
    {
      return nullptr;
    }
    BB_DEFER(fclose(input));
    return ReadWholeFile(input, pSize);
  }

} // namespace bb
