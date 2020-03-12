#include <fstream>
#include <type_traits>
#include <clocale>

#include <common.hpp>
#include <config.hpp>

extern "C"
{
#include <token.h>
#include <config.lex.h>

#if _WIN32

  locale_t clocale()
  {
    return _create_locale(LC_ALL, "C");
  }

  void freelocale(locale_t locale)
  {
    _free_locale(locale);
  }

  double strtod_l(const char* strSource, char** endptr, locale_t locale)
  {
    return _strtod_l(strSource, endptr, locale);
  }
  
#else
  
  locale_t clocale()
  {
    return newlocale(LC_ALL_MASK, "C", nullptr);
  }
 
#endif
  

}

namespace bb
{

  config_t::config_t(config_t&& move)
  :dict(std::move(move.dict))
  {
    ;
  }

  config_t& config_t::operator=(config_t&& move)
  {
    if (this == &move)
    {
      return *this;
    }

    this->dict = std::move(move.dict);
    return *this;
  }

  config_t::config_t()
  {
    ;
  }

  config_t::config_t(const std::string& filename)
  {
    this->Load(filename);
  }

  config_t::~config_t()
  {
    ;
  }

  static void ThrowTokenError(int token, const bbCoreConfigTokenData_t& tokenData)
  {
    switch (tokenData.type)
    {
    case BB_STRING:
      bb::Error("Unexpected token: %d (\"%s\")", token, tokenData.str);
      break;
    case BB_NUMBER:
      bb::Error("Unexpected token: %d (\"%e\")", token, tokenData.num);
      break;
    default:
      break;
    }
    throw std::runtime_error("Invalid configuration line");
  }

  std::string config_t::ParseString(const std::string& line, const std::string& context)
  {
    if (line.empty())
    {
      return context;
    }

    locale_t cLocale = clocale();
    if (cLocale == ((locale_t)0))
    {
      throw std::runtime_error("Can't initialize locale");
    }
    BB_DEFER(freelocale(cLocale));

    yyscan_t scanner;
    if (yylex_init(&scanner) != 0)
    {
      throw std::runtime_error("Can't initialize scanner");
    }
    BB_DEFER(yylex_destroy(scanner));

    bbCoreConfigTokenData_t tokenData;
    tokenData.type = BB_ERROR;
    tokenData.str = nullptr;
    BB_DEFER(bbCoreConfigTokenReset(&tokenData));

    yyset_extra(&tokenData, scanner);

    // this can lead to YY_FATAL_ERROR, and exit
    auto tmpBuf = yy_scan_string(line.c_str(), scanner);
    BB_DEFER(yy_delete_buffer(tmpBuf, scanner));

    // Only this variants expected
    // }
    // "key" {
    // "key": value
    // "key": "value"

    // Expect "key" or }
    int token = yylex(scanner, cLocale);

    std::string key;
    switch (token)
    {
      case BB_STRING:
        key = tokenData.str;
        break;
      case BB_FINISH:
        {
          if (context.empty())
          {
            bb::Error("Unexpected context close");
            throw std::runtime_error("Unexpected context close");
          }

          auto dotPos = context.rfind('.');
          if (dotPos == std::string::npos)
          {
            dotPos = 0;
          }
          return context.substr(0, dotPos);
        }
      default:
        ThrowTokenError(token, tokenData);
    }
    bbCoreConfigTokenReset(&tokenData);

    // expect : or {
    token = yylex(scanner, cLocale);

    switch(token)
    {
      case BB_SET:
        // just go to next token
        break;
      case BB_START:
        // new context started, just return
        return  (context.empty())?(key):(context + "." + key);
      default:
        ThrowTokenError(token, tokenData);
    }
    bbCoreConfigTokenReset(&tokenData);

    std::string fullKey = (context.empty())?(key):(context + "." + key);

    switch (yylex(scanner, cLocale))
    {
    case BB_STRING:
      this->dict[fullKey] = ref_t::String(tokenData.str);
      break;
    case BB_NUMBER:
      this->dict[fullKey] = ref_t::Number(tokenData.num);
      break;
    default:
      throw std::runtime_error("Invalid configuration line");
    }

    return context;
  }

  void config_t::Load(const std::string& filename)
  {
    std::ifstream input(filename);
    std::string context;

    if (input)
    {
      std::string line;
      while(std::getline(input, line))
      {
        context = this->ParseString(line, context);
      }
    }
    else
    {
      throw std::runtime_error(std::string("Can't open file '") + filename + std::string("'"));
    }
  }

  void config_t::Save(const std::string& filename) const
  {
    std::ofstream output(filename);
    if (output)
    {
      for (auto it = this->dict.begin(), e = this->dict.end(); it != e; ++it)
      {
        output << '\"' << it->first << "\": " << it->second.ToString() << std::endl;
      }
    }
    else
    {
      throw std::runtime_error(std::string("Can't open file '") + filename + std::string("'"));
    }
  }

} // namespace bb
