#include <fstream>
#include <type_traits>
#include <clocale>

#include <common.hpp>
#include <config.hpp>

extern "C"
{
#include <token.h>
#include <config.lex.h>
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

  void config_t::ParseString(const std::string& line)
  {
    if (line.empty())
    {
      return;
    }


    locale_t c_locale = newlocale(LC_ALL_MASK, "C", NULL);
    if (c_locale == ((locale_t)0))
    {
      throw std::runtime_error("Can't initialize locale");
    }
    BB_DEFER(freelocale(c_locale));

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
    // "key": value
    // "key": "value"

    // expect key
    if (yylex(scanner, c_locale) != BB_STRING)
    {
      throw std::runtime_error("Invalid configuration line");
    }

    std::string key = tokenData.str;
    bbCoreConfigTokenReset(&tokenData);

    //expect :
    if (yylex(scanner, c_locale) != BB_SET)
    {
      throw std::runtime_error("Invalid configuration line");
    }
    bbCoreConfigTokenReset(&tokenData);

    switch (yylex(scanner, c_locale))
    {
    case BB_STRING:
      this->dict[key] = ref_t::String(tokenData.str);
      break;
    case BB_NUMBER:
      this->dict[key] = ref_t::Number(tokenData.num);
      break;
    default:
      throw std::runtime_error("Invalid configuration line");
    }
  }

  void config_t::Load(const std::string& filename)
  {
    std::ifstream input(filename);
    if (input)
    {
      std::string line;
      while(std::getline(input, line))
      {
        this->ParseString(line);
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
