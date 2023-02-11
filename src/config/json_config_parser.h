// Create: 2023-02-09
// Author: CasinoHe
// Purpose: a json config parser
#pragma once

#include "log/logger.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <string>
#include <memory>

namespace multiplayer_server
{
  class JsonConfigParser
  {
  public:
    JsonConfigParser(const std::string &config_file_path);
    ~JsonConfigParser();

    // Parse config file
    [[nodiscard]] bool parse();

    // Get config value
    template <typename T>
    T get(const std::string &key, const T &default_value) const 
    {
      return config_tree_.get<T>(key, default_value);
    }

  private:
    std::string config_file_path_;
    boost::property_tree::ptree config_tree_;
    std::shared_ptr<LoggerImp> logger_;
  };
}