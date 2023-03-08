#include "game_config.h"
#include <type_traits>
#include <filesystem>
#include <fstream>
#include <tuple>

#define SERVER_CONFIG_STR "server"
#define LOG_CONFIG_STR "logger"

namespace multiplayer_server
{
  GameConfig::GameConfig(const std::string &config_file_path)
  {
    // first, init logger
    logger_ = g_logger_manager.create_logger("GameConfig", LoggerLevel::Debug, "log/game_config.log");

    // second, load config from file
    load_config(config_file_path);

    // third, save config file path
    config_file_path_ = config_file_path;
  }

  GameConfig::~GameConfig()
  {
  }

  // return tuple of server ip and port
  std::shared_ptr<std::tuple<std::string, int>> GameConfig::get_server_ip_port()
  {
    auto iter = config_.find(SERVER_CONFIG_STR);
    if (iter == config_.end())
    {
      return nullptr;
    }

    return std::static_pointer_cast<std::tuple<std::string, int>>(iter->second);
  }

  void GameConfig::reload_config()
  {
    // reload config from file
    load_config(config_file_path_);
  }

  void GameConfig::load_config(const std::string &config_file_path)
  {
    // load config from file, this file is a json file
    // first, check the legality of the file
    if (!std::filesystem::exists(config_file_path))
    {
      logger_->error("config file not exist, path: {}", config_file_path);
      return;
    }

    if (!std::filesystem::is_regular_file(config_file_path))
    {
      logger_->error("config file is not a regular file, path: {}", config_file_path);
      return;
    }

    // check the extension of the file
    if (std::filesystem::path(config_file_path).extension().compare(".json") != 0)
    {
      logger_->error("config file is not a json file, path: {}", config_file_path);
      return;
    }

    // second, parse the config file
    JsonTree config_tree;
#ifdef USE_BOOST_JSON_PARSER
    try
    {
      boost::property_tree::read_json(config_file_path, config_tree);
    }
    catch (const boost::property_tree::json_parser::json_parser_error &e)
    {
      logger_->error("parse config file failed, error: {}", e.what());
      return;
    }
#elif USE_RAPIDJSON
    std::ifstream ifs(config_file_path);
    if (!ifs.is_open())
    {
      logger_->error("open config file failed, path: {}", config_file_path);
      return;
    }
    std::string config_str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    config_tree.Parse(config_str.c_str());
    if (config_tree.HasParseError())
    {
      logger_->error("parse config file failed");
      return;
    }
#endif

    // then, call every load function to load data from config file
    load_server_config(config_tree);
    load_logger_config(config_tree);
  }

  // load server config
  void GameConfig::load_server_config(const JsonTree &config_tree)
  {
    // load server config
    // first, check the existence of server config
#ifdef USE_BOOST_JSON_PARSER
    if (config_tree.find(SERVER_CONFIG_STR) == config_tree.not_found())
    {
      logger_->error("server config not exist");
      return;
    }
#elif USE_RAPIDJSON
    if (!config_tree.HasMember(SERVER_CONFIG_STR))
    {
      logger_->error("server config not exist");
      return;
    }
#endif

    // second, check the type of server config
#ifdef USE_BOOST_JSON_PARSER
    if (!config_tree[SERVER_CONFIG_STR].is_object())
    {
      logger_->error("server config is not a object");
      return;
    }
#elif USE_RAPIDJSON
    if (!config_tree[SERVER_CONFIG_STR].IsObject())
    {
      logger_->error("server config is not a object");
      return;
    }
#endif

    // third, load server config
#ifdef USE_BOOST_JSON_PARSER
    const JsonNode &server_config = config_tree.get_child(SERVER_CONFIG_STR);
#elif USE_RAPIDJSON
    const JsonNode &server_config = config_tree[SERVER_CONFIG_STR];
#endif

    // load server ip
#ifdef USE_BOOST_JSON_PARSER
    if (server_config.find("ip") == server_config.not_found())
    {
      logger_->error("server ip not exist");
      return;
    }

    if (!server_config.get_child("ip").is_data())
    {
      logger_->error("server ip is not a string");
      return;
    }

    auto server_ip = server_config.get<std::string>("ip");
#elif USE_RAPIDJSON
    if (!server_config.HasMember("ip"))
    {
      logger_->error("server ip not exist");
      return;
    }

    if (!server_config["ip"].IsString())
    {
      logger_->error("server ip is not a string");
      return;
    }

    auto server_ip = server_config["ip"].GetString();
#endif

    // load server port
#ifdef USE_BOOST_JSON_PARSER
    if (server_config.find("port") == server_config.not_found())
    {
      logger_->error("server port not exist");
      return;
    }

    if (!server_config.get_child("port").is_data())
    {
      logger_->error("server port is not a number");
      return;
    }

    int server_port = server_config.get<int>("port");
#elif USE_RAPIDJSON
    if (!server_config.HasMember("port"))
    {
      logger_->error("server port not exist");
      return;
    }

    if (!server_config["port"].IsNumber())
    {
      logger_->error("server port is not a number");
      return;
    }

    int server_port = server_config["port"].GetInt();
#endif

    // make a shared_ptr of tuple to store server config
    auto server_config_ptr = std::make_shared<std::tuple<std::string, int>>(server_ip, server_port);
    config_["server"] = std::static_pointer_cast<void>(server_config_ptr);
  }

  // load logger config
  void GameConfig::load_logger_config(const JsonTree &config_tree)
  {
    // load logger config
    // first, check the existence of server config
#ifdef USE_BOOST_JSON_PARSER
    if (config_tree.find(LOG_CONFIG_STR) == config_tree.not_found())
    {
      logger_->error("server config not exist");
      return;
    }
#elif USE_RAPIDJSON
    if (!config_tree.HasMember(LOG_CONFIG_STR))
    {
      logger_->error("server config not exist");
      return;
    }
#endif
  }
}