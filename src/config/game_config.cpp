#include "game_config.h"
#include <type_traits>
#include <filesystem>
#include <fstream>
#include <tuple>
#include <optional>
#include <stdexcept>

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
    load_login_config(config_tree);
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
#ifdef USE_RAPIDJSON
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
    config_[SERVER_CONFIG_STR] = std::static_pointer_cast<void>(server_config_ptr);
  }

  // load logger config
  void GameConfig::load_logger_config(const JsonTree &config_tree)
  {
    // load logger config
    // first, check the existence of server config
#ifdef USE_BOOST_JSON_PARSER
    if (config_tree.find(LOG_CONFIG_STR) == config_tree.not_found())
    {
      logger_->error("logger config not exist");
      return;
    }

    const JsonNode &logger_config = config_tree.get_child(LOG_CONFIG_STR);
#elif USE_RAPIDJSON
    if (!config_tree.HasMember(LOG_CONFIG_STR))
    {
      logger_->error("logger config not exist");
      return;
    }
    const JsonNode &logger_config = config_tree[LOG_CONFIG_STR];
#endif

    // logger config is a two layers json object, example:
    //   
    // "logger": [
    // 	{
    // 		"name": "main_log",
    // 		"file": "log/main.log",
    // 		"level": "debug"
    // 	},
    // 	{
    // 		"name": "config_log",
    // 		"file": "log/config.log",
    // 		"level": "debug"
    // 	}
    // ]

    // traverse the entire list logger config
#ifdef USE_BOOST_JSON_PARSER
    for (const auto &logger : logger_config)
    {
      load_single_logger_config(logger.second);
    }
#elif USE_RAPIDJSON
    for (const auto &logger : logger_config.GetArray())
    {
      load_single_logger_config(logger);
    }
#endif
  }

  void GameConfig::load_single_logger_config(const JsonNode &logger)
  {
#ifdef USE_BOOST_JSON_PARSER
    // load logger name
    if (logger.find("name") == logger.not_found())
    {
      logger_->error("logger name not exist");
      return;
    }

    std::string logger_name = logger.get<std::string>("name");
#elif USE_RAPIDJSON
    // check if logger is a object
    if (!logger.IsObject())
    {
      logger_->error("logger is not a object");
      return;
    }

    // load logger name
    if (!logger.HasMember("name"))
    {
      logger_->error("logger name not exist");
      return;
    }

    if (!logger["name"].IsString())
    {
      logger_->error("logger name is not a string");
      return;
    }

    std::string logger_name = logger["name"].GetString();
#endif

    if (logger_name.empty())
    {
      logger_->error("logger name is empty");
      return;
    }

    auto data_ptr = std::make_shared<LoggerImp::config_map_type>();

#ifdef USE_BOOST_JSON_PARSER
    // if logger time
    if (logger.find("log_time") != logger.not_found())
    {
      data_ptr->emplace("log_time", logger.get<bool>("log_time"));
    }

    // if logger thread
    if (logger.find("log_thread") != logger.not_found())
    {
      data_ptr->emplace("log_thread", logger.get<bool>("log_thread"));
    }

    // if logger log_level
    if (logger.find("log_level") != logger.not_found())
    {
      data_ptr->emplace("log_level", logger.get<bool>("log_level"));
    }

    // if logger log_source
    if (logger.find("log_source") != logger.not_found())
    {
      data_ptr->emplace("log_source", logger.get<bool>("log_source"));
    }

    // if logger log_line
    if (logger.find("log_line") != logger.not_found())
    {
      data_ptr->emplace("log_line", logger.get<bool>("log_line"));
    }

    // if logger log_funcname
    if (logger.find("log_funcname") != logger.not_found())
    {
      data_ptr->emplace("log_funcname", logger.get<bool>("log_funcname"));
    }

    // if logger tag_name
    if (logger.find("log_tag_name") != logger.not_found())
    {
      data_ptr->emplace("log_tag_name", logger.get<std::string>("log_tag_name"));
    }

    // if logger level
    if (logger.find("level") != logger.not_found())
    {
      data_ptr->emplace("level", logger.get<std::string>("level"));
    }

    // get log_file_max_size
    if (logger.find("log_file_max_size") != logger.not_found())
    {
      data_ptr->emplace("log_file_max_size", logger.get<size_t>("log_file_max_size"));
    }

    // log_file_max_number
    if (logger.find("log_file_max_number") != logger.not_found())
    {
      data_ptr->emplace("log_file_max_number", logger.get<size_t>("log_file_max_number"));
    }

    // get log_file_name
    if (logger.find("log_file_name") != logger.not_found())
    {
      data_ptr->emplace("log_file_name", logger.get<std::string>("log_file_name"));
    }
    // get log_file_dir
    if (logger.find("log_file_dir") != logger.not_found())
    {
      data_ptr->emplace("log_file_dir", logger.get<std::string>("log_file_dir"));
    }
    // get log_file_extension
    if (logger.find("log_file_extension") != logger.not_found())
    {
      data_ptr->emplace("log_file_extension", logger.get<std::string>("log_file_extension"));
    }

#elif USE_RAPIDJSON
    // if logger time
    if (logger.HasMember("log_time") && logger["log_time"].IsBool())
    {
      data_ptr->emplace("log_time", logger["log_time"].GetBool());
    }

    // if log thread
    if (logger.HasMember("log_thread") && logger["log_thread"].IsBool())
    {
      data_ptr->emplace("log_thread", logger["log_thread"].GetBool());
    }

    // if log log_level
    if (logger.HasMember("log_level") && logger["log_level"].IsBool())
    {
      data_ptr->emplace("log_level", logger["log_level"].GetBool());
    }

    // if log log_source
    if (logger.HasMember("log_source") && logger["log_source"].IsBool())
    {
      data_ptr->emplace("log_source", logger["log_source"].GetBool());
    }

    // if log log_line
    if (logger.HasMember("log_line") && logger["log_line"].IsBool())
    {
      data_ptr->emplace("log_line", logger["log_line"].GetBool());
    }
    
    // if log funcname
    if (logger.HasMember("log_funcname") && logger["log_funcname"].IsBool())
    {
      data_ptr->emplace("log_funcname", logger["log_funcname"].GetBool());
    }

    // get level
    if (logger.HasMember("level") && logger["level"].IsString())
    {
      data_ptr->emplace("level", logger["level"].GetString());
    }

    // get log_tag_name
    if (logger.HasMember("log_tag_name") && logger["log_tag_name"].IsString())
    {
      data_ptr->emplace("log_tag_name", logger["log_tag_name"].GetString());
    }

    // get log_file_max_size
    if (logger.HasMember("log_file_max_size") && logger["log_file_max_size"].IsInt())
    {
      data_ptr->emplace("log_file_max_size", static_cast<size_t>(logger["log_file_max_size"].GetUint()));
    }
    
    // get log_file_max_number
    if (logger.HasMember("log_file_max_number") && logger["log_file_max_number"].IsInt())
    {
      data_ptr->emplace("log_file_max_number", static_cast<size_t>(logger["log_file_max_number"].GetUint()));
    }

    // get log_file_name
    if (logger.HasMember("log_file_name") && logger["log_file_name"].IsString())
    {
      data_ptr->emplace("log_file_name", logger["log_file_name"].GetString());
    }

    // get log_file_dir
    if (logger.HasMember("log_file_dir") && logger["log_file_dir"].IsString())
    {
      data_ptr->emplace("log_file_dir", logger["log_file_dir"].GetString());
    }

    // get log_file_extension
    if (logger.HasMember("log_file_extension") && logger["log_file_extension"].IsString())
    {
      data_ptr->emplace("log_file_extension", logger["log_file_extension"].GetString());
    }

#endif

    // insert data ptr into config_ to store logger config
    config_[LOG_CONFIG_STR + logger_name] = std::static_pointer_cast<void>(data_ptr);
  }

  // load login configuration
  void GameConfig::load_login_config(const JsonTree &config_tree)
  {
    // load login config
    // first, check the existence of login config
#ifdef USE_BOOST_JSON_PARSER
    if (config_tree.find(LOGIN_CONFIG_STR) == config_tree.not_found())
    {
      logger_->error("login config not exist");
      throw std::runtime_error("login config not exist");
      return;
    }
#elif USE_RAPIDJSON
    if (!config_tree.HasMember(LOGIN_CONFIG_STR))
    {
      logger_->error("login config not exist");
      throw std::runtime_error("login config not exist");
      return;
    }
#endif

    auto data_ptr = std::make_shared<LoginEntityConfig>();

    // second, get login config details
#ifdef USE_BOOST_JSON_PARSER
    const auto &login_config = config_tree.get_child(LOGIN_CONFIG_STR);
#elif USE_RAPIDJSON
    const auto &login_config = config_tree[LOGIN_CONFIG_STR];
#endif
  
    // third, check the existence of login config details
#ifdef USE_BOOST_JSON_PARSER
    // entity class
    if (login_config.find("entity") == login_config.not_found())
    {
      logger_->error("login entity config not exist");
      throw std::runtime_error("login entity config not exist");
      return;
    }
    data_ptr->entity_class_name = login_config["entity"].GetString();

    // entity type
    if (login_config.find("entity_type") == login_config.not_found())
    {
      logger_->error("login entity type config not exist");
      throw std::runtime_error("login entity config not exist");
      return;
    }
    data_ptr->entity_type = login_config["entity_type"].GetString();

    if (login_config.find("components") != login_config.not_found())
    {
      for (const auto &component : login_config.get_child("components"))
      {
        data_ptr->entity_components.emplace_back(component.get<std::string>());
      }
    }
#elif USE_RAPIDJSON
    // get entity class
    if (!login_config.HasMember("entity") || !login_config["entity"].IsString())
    {
      logger_->error("login entity config not exist");
      throw std::runtime_error("login entity config not exist");
      return;
    }
    data_ptr->entity_class_name = login_config["entity"].GetString();

    // get entity type
    if (!login_config.HasMember("entity_type") || !login_config["entity"].IsString())
    {
      logger_->error("login entity type config not exist");
      throw std::runtime_error("login entity type config not exist");
      return;
    }
    data_ptr->entity_type = login_config["entity_type"].GetString();

    // get entity components, it's a list
    if (login_config.HasMember("components") && login_config["components"].IsArray())
    {
      for (const auto &component : login_config["components"].GetArray())
      {
        data_ptr->entity_components.emplace_back(component.GetString());
      }
    }
#endif
    config_[LOGIN_CONFIG_STR] = std::static_pointer_cast<void>(data_ptr);
  }
}