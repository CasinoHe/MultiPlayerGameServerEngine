#include "json_config_parser.h"
#include <fstream>
#include <filesystem>

namespace multiplayer_server
{
  JsonConfigParser::JsonConfigParser(const std::string &config_file_path)
      : config_file_path_(config_file_path)
  {
    logger_ = g_logger_manager.create_logger("JsonConfigParser", LoggerLevel::Debug, "log/json_config.log");
  }

  JsonConfigParser::~JsonConfigParser()
  {
  }

  bool JsonConfigParser::parse()
  {
#ifdef USE_BOOST_JSON_PARSER
    try
    {
      boost::property_tree::read_json(config_file_path_, config_tree_);
    }
    catch (const boost::property_tree::json_parser::json_parser_error &e)
    {
      logger_->error("JsonConfigParser::parse() failed, error: {}", e.what());
      return false;
    }
#endif

#ifdef USE_RAPIDJSON
    // check the existence of config file
    if (!std::filesystem::exists(config_file_path_))
    {
      logger_->error("JsonConfigParser::parse() failed, error: {}", "config file not exist");
      return false;
    }
    // check the config file is a regular file
    if (!std::filesystem::is_regular_file(config_file_path_))
    {
      logger_->error("JsonConfigParser::parse() failed, error: {}", "config file is not a regular file");
      return false;
    }

    // check the config file is a json file
    if (std::filesystem::path(config_file_path_).extension().compare(".json") != 0)
    {
      logger_->error("JsonConfigParser::parse() failed, error: {}", "config file is not a json file");
      return false;
    }

    std::ifstream ifs(config_file_path_);
    if (!ifs.is_open())
    {
      logger_->error("JsonConfigParser::parse() failed, error: {}", "open config file failed");
      return false;
    }
    std::string config_str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    config_tree_.Parse(config_str.c_str());
    if (config_tree_.HasParseError())
    {
      logger_->error("JsonConfigParser::parse() failed, error: {}", "parse config file failed");
      return false;
    }
#endif

    return true;
  }
}