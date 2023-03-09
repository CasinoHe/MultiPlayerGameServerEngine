// Created: 2023.03.08
// Author: CasinoHe
// Purpose: load and parse game config, and provide a interface to get config value
#pragma once

#include "log/logger.h"
#ifdef USE_BOOST_JSON_PARSER
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#elif USE_RAPIDJSON
#include <rapidjson/document.h>
#endif
#include <string>
#include <memory>
#include <unordered_map>

namespace multiplayer_server
{
#ifdef USE_BOOST_JSON_PARSER
  using JsonTree = boost::property_tree::ptree;
  using JsonNode = boost::property_tree::ptree;
#elif USE_RAPIDJSON
  using JsonTree = rapidjson::Document;
  using JsonNode = rapidjson::Value;
#endif

  class GameConfig
  {
  public:
    GameConfig(const std::string &config_file_path);
    ~GameConfig();

    // non-copyable
    GameConfig(const GameConfig&) = delete;
    GameConfig& operator=(const GameConfig&) = delete;
    GameConfig(GameConfig&&) = delete;
    GameConfig& operator=(GameConfig&&) = delete;

  public:
    // get config value
    template <typename T>
    const std::shared_ptr<T> get(const std::string &key, const std::shared_ptr<T> &default_value = nullptr) const
    {
      auto it = config_.find(key);
      if (it != config_.end())
      {
        return std::static_pointer_cast<T>(it->second);
      }
      else
      {
        return default_value;
      }
    }

    // get server ip and server port
    std::shared_ptr<std::tuple<std::string, int>> get_server_ip_port();

  private:
    // load config from file
    void load_config(const std::string &config_file_path);
    // reload config
    void reload_config();

    // load server config data, including ip, port...
    void load_server_config(const JsonTree &tree);
    // load game logger config data, including log level, log file path...
    void load_logger_config(const JsonTree &tree);
    void load_single_logger_config(const JsonNode &node);

  private:
    // config node
    std::unordered_map<std::string, std::shared_ptr<void>> config_;

    // logger
    std::shared_ptr<LoggerImp> logger_;
    
    // save config file path
    std::string config_file_path_;
  };
}