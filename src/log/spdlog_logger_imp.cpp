#include "spdlog_logger_imp.h"
#include "spdlog/sinks/stdout_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "config/json_config_parser.h"
#ifdef USE_FMT
#include <fmt/core.h>
#include <fmt/chrono.h>
#include <fmt/ranges.h>
#endif
#include <filesystem>
#include <string>
#include <map>
#include <iostream>

namespace multiplayer_server
{
  SpdlogLoggerImp::SpdlogLoggerImp(const std::string &tagname, const std::string &log_path, const std::string &config_file_path) : LoggerImp(tagname, log_path, config_file_path)
  {
    init(config_file_path);
  }

  SpdlogLoggerImp::~SpdlogLoggerImp()
  {
  }

  bool SpdlogLoggerImp::read_config(const std::string &config_file_path)
  {
    // check config file path
    if (!std::filesystem::exists(config_file_path))
    {
      return false;
    }

    JsonConfigParser parser(config_file_path);
    if (!parser.parse())
    {
      return false;
    }

    // get log time
    need_log_time_ = parser.get<bool>("log_time", true);

    // get log thread
    need_log_thread_ = parser.get<bool>("log_thread", true);

    // get log level
    need_log_level_ = parser.get<bool>("log_level", true);

    // get_log_source
    need_log_source_ = parser.get<bool>("log_source", true);

    // get log line
    need_log_line_ = parser.get<bool>("log_line", true);

    // get log function name
    need_log_funcname_ = parser.get<bool>("log_funcname", true);

    // get log level
    std::string level = parser.get<std::string>("level", "off");
    std::map<std::string, LoggerLevel> level_map = {
      {"debug", LoggerLevel::Debug},
      {"info", LoggerLevel::Info},
      {"warn", LoggerLevel::Warn},
      {"error", LoggerLevel::Error},
      {"critical", LoggerLevel::Critical}
    };
    if (level_map.find(level) != level_map.end())
    {
      level_ = level_map[level];
    }

    // get log tag name
    tag_name_ = parser.get<std::string>("log_tag_name", "major");

    // get log file max size
    log_file_size_ = parser.get<size_t>("log_file_max_size", 0);

    // get log file max number
    log_file_cnt_ = parser.get<size_t>("log_file_max_number", 3);

    // get log file name
    std::string log_file_name = parser.get<std::string>("log_file_name", "");

    // get log file dir
    std::string log_file_dir = parser.get<std::string>("log_file_dir", "");

    // get log file extension
    std::string log_file_extension = parser.get<std::string>("log_file_extension", "");

    // get log file full path
    if (!log_file_name.empty())
    {
      file_path_ = log_file_dir + "/" + log_file_name + "." + log_file_extension;
    }
    else
    {
      file_path_ = "";
    }

    // get log console
    // bool log_console = parser.get<bool>("log_console", true);

    // get log console color
    // bool log_console_color = parser.get<bool>("log_console_color", true);

    return true;
  }

  bool SpdlogLoggerImp::init(const std::string &config_file_path)
  {
    read_config(config_file_path);

    std::string pattern("");

    // generate log pattern
    if (need_log_time_)
    {
      pattern += "[%Y-%m-%d %H:%M:%S.%e] ";
    }
    if (!tag_name_.empty())
    {
      pattern += "[%n] ";
    }
    if (need_log_thread_)
    {
      pattern += "[%t] ";
    }
    if (need_log_level_)
    {
      pattern += "[%l] ";
    }
    if (need_log_source_)
    {
      pattern += "[%s] ";
    }
    if (need_log_funcname_)
    {
      pattern += "[%#] ";
    }
    if (need_log_line_)
    {
      pattern += "[%!] ";
    }
    pattern += "%v";

    // init spdlog logger
    if (file_path_.empty())
    {
      // no filename, use stdout
      logger_ = std::make_shared<spdlog::logger>(tag_name_, std::make_shared<spdlog::sinks::stdout_sink_mt>());
    }
    else
    {
      // use file sink
      if (log_file_size_ > 0)
      {
        // set max size
        logger_ = std::make_shared<spdlog::logger>(tag_name_, std::make_shared<spdlog::sinks::rotating_file_sink_mt>(file_path_, log_file_size_, log_file_cnt_, true));
      }
      else
      {
        // no max size
        logger_ = std::make_shared<spdlog::logger>(tag_name_, std::make_shared<spdlog::sinks::basic_file_sink_mt>(file_path_, false));
      }
    }

    // set logger level
    set_level(level_);
    logger_->set_pattern(pattern);

    return true;
  }

  void SpdlogLoggerImp::set_level(LoggerLevel level)
  {
    // set base class level
    LoggerImp::set_level(level);

    // set spdlog level
    std::map<LoggerLevel, spdlog::level::level_enum> level_map = {
        {LoggerLevel::Debug, spdlog::level::debug},
        {LoggerLevel::Info, spdlog::level::info},
        {LoggerLevel::Warn, spdlog::level::warn},
        {LoggerLevel::Error, spdlog::level::err},
        {LoggerLevel::Critical, spdlog::level::critical}};

    // set logger level
    if (logger_ && level_map.find(level) != level_map.end())
    {
      logger_->set_level(level_map[level]);
      logger_->flush_on(level_map[level]);
    }
  }

  void SpdlogLoggerImp::debug(const std::string &msg)
  {
    if (logger_)
    {
      logger_->debug(msg);
    }
    else
    {
      spdlog::debug(msg);
    }
  }

  void SpdlogLoggerImp::info(const std::string &msg)
  {
    if (logger_)
    {
      logger_->info(msg);
    }
    else
    {
      spdlog::info(msg);
    }
  }

  void SpdlogLoggerImp::warn(const std::string &msg)
  {
    if (logger_)
    {
      logger_->warn(msg);
    }
    else
    {
      spdlog::warn(msg);
    }
  }

  void SpdlogLoggerImp::error(const std::string &msg)
  {
    if (logger_)
    {
      logger_->error(msg);
    }
    else
    {
      spdlog::error(msg);
    }
  }

  void SpdlogLoggerImp::critical(const std::string &msg)
  {
    if (logger_)
    {
      logger_->critical(msg);
    }
    else
    {
      spdlog::critical(msg);
    }
  }
}