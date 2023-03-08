#include "spdlog_logger_imp.h"
#include "spdlog/sinks/stdout_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#ifdef USE_FMT
#include <fmt/core.h>
#include <fmt/chrono.h>
#include <fmt/ranges.h>
#endif
#include <string>
#include <map>
#include <iostream>

namespace multiplayer_server
{
  SpdlogLoggerImp::SpdlogLoggerImp(const std::string &tagname, const std::string &log_path, LoggerImp::config_type config) : LoggerImp(tagname, log_path, config)
  {
    init();
  }

  SpdlogLoggerImp::~SpdlogLoggerImp()
  {
  }

  bool SpdlogLoggerImp::read_config()
  {
    // check config exists or not
    if (!config_)
    {
      return false;
    }

    // get log time
    auto iter = config_->find("log_time");
    if (iter != config_->end())
    {
      need_log_time_ = std::get<bool>(iter->second);
    }

    // get log thread
    iter = config_->find("log_thread");
    if (iter != config_->end())
    {
      need_log_thread_ = std::get<bool>(iter->second);
    }

    // get log level
    iter = config_->find("log_level");
    if (iter != config_->end())
    {
      need_log_level_ = std::get<bool>(iter->second);
    }

    // get_log_source
    iter = config_->find("log_source");
    if (iter != config_->end())
    {
      need_log_source_ = std::get<bool>(iter->second);
    }

    // get log line
    iter = config_->find("log_line");
    if (iter != config_->end())
    {
      need_log_line_ = std::get<bool>(iter->second);
    }

    // get log function name
    iter = config_->find("log_funcname");
    if (iter != config_->end())
    {
      need_log_funcname_ = std::get<bool>(iter->second);
    }

    // get log level
    iter = config_->find("level");
    if (iter != config_->end())
    {
      std::string level = std::get<std::string>(iter->second);
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
    }

    // get log tag name
    iter = config_->find("log_tag_name");
    if (iter != config_->end())
    {
      tag_name_ = std::get<std::string>(iter->second);
    }

    // get log file max size
    iter = config_->find("log_file_max_size");
    if (iter != config_->end())
    {
      log_file_size_ = std::get<size_t>(iter->second);
    }

    // get log file max number
    iter = config_->find("log_file_max_number");
    if (iter != config_->end())
    {
      log_file_cnt_ = std::get<size_t>(iter->second);
    }

    // get log file name
    iter = config_->find("log_file_name");
    std::string log_file_name("");
    if (iter != config_->end())
    {
      log_file_name = std::get<std::string>(iter->second);
    }

    // get log file dir
    iter = config_->find("log_file_dir");
    std::string log_file_dir("");
    if (iter != config_->end())
    {
      log_file_dir = std::get<std::string>(iter->second);
    }

    // get log file extension
    iter = config_->find("log_file_extension");
    std::string log_file_extension;
    if (iter != config_->end())
    {
      log_file_extension = std::get<std::string>(iter->second);
    }

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
    // iter = config_->find("log_console");
    // if (iter != config_->end())
    // {
    //   need_log_console_ = std::get<bool>(iter->second);
    // }

    // get log console color
    // iter = config_->find("log_console_color");
    // if (iter != config_->end())
    // {
    //   need_log_console_color_ = std::get<bool>(iter->second);
    // }

    return true;
  }

  bool SpdlogLoggerImp::init()
  {
    read_config();

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