#include "json_config_parser.h"

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
    try
    {
      boost::property_tree::read_json(config_file_path_, config_tree_);
    }
    catch (const boost::property_tree::json_parser::json_parser_error &e)
    {
      logger_->error("JsonConfigParser::parse() failed, error: {}", e.what());
      return false;
    }

    return true;
  }
}