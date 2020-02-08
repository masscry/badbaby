/**
 * @file config.hpp
 * 
 * Simple text config file routines
 * 
 */

#pragma once
#ifndef __BB_COMMON_CONFIG_HEADER__
#define __BB_COMMON_CONFIG_HEADER__

#include <value.hpp>
#include <common.hpp>

#include <unordered_map>

namespace bb
{

  class config_t final
  {
    using dict_t = std::unordered_map<std::string, ref_t>;

    dict_t dict;

    config_t(const config_t&) = delete;
    config_t& operator=(const config_t&) = delete;

    void ParseString(const std::string& line);

  public:

    void Load(const std::string& filename);

    void Save(const std::string& filename) const;

    ref_t& operator [] (const std::string& key)
    {
      return this->dict[key];
    }

    const ref_t& operator [] (const std::string& key) const
    {
      auto it = this->dict.find(key);
      if (it == this->dict.end())
      {
        throw std::runtime_error(std::string("Key '") + key + std::string("' not found"));
      }
      return it->second;
    }

    double Value(const std::string& key, double defaultVal) const
    {
      auto it = this->dict.find(key);
      if (it == this->dict.end())
      {
        bb::Debug("\"%s\" is not found in config (defaults to %f)", key.c_str(), defaultVal);
        return defaultVal;
      }
      if (it->second.Type() != type_t::number)
      {
        bb::Debug("\"%s\" has invalid type (defaults to %f)", key.c_str(), defaultVal);
        return defaultVal;
      }
      return it->second.Number();
    }

    const std::string& Value(const std::string& key, const std::string& defaultVal) const
    {
      auto it = this->dict.find(key);
      if (it == this->dict.end())
      {
        bb::Debug("\"%s\" is not found in config (defaults to %s)", key.c_str(), defaultVal.c_str());
        return defaultVal;
      }
      if (it->second.Type() != type_t::string)
      {
        bb::Debug("\"%s\" has invalid type (defaults to %s)", key.c_str(), defaultVal.c_str());
        return defaultVal;
      }
      return it->second.String();
    }

    config_t(config_t&&);
    config_t& operator=(config_t&&);

    config_t();
    config_t(const std::string& filename);
    ~config_t();
  };

} // namespace bb

#endif /* __BB_COMMON_CONFIG_HEADER__ */