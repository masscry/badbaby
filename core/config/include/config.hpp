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

namespace bb
{

  class config_t final
  {
  public:

    ref_t& operator [] (const std::string& key);

    const ref_t& operator [] (const std::string& key) const;

  };

} // namespace bb

#endif /* __BB_COMMON_CONFIG_HEADER__ */