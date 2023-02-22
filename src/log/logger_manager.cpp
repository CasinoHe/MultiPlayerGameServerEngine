#include "logger_manager.h"
#ifdef USE_SPDLOG
#include "spdlog_logger_imp.h"
#endif

namespace multiplayer_server
{
  LoggerManager &LoggerManager::get_instance()
  {
    // static std::shared_ptr<LoggerManager> instance = std::make_shared<LoggerManager>();
    static LoggerManager instance;
    return instance;
  }

  LoggerManager::LoggerManager()
  {
#ifdef USE_SPDLOG
    default_logger_ = std::make_shared<SpdlogLoggerImp>("major");
#else
    static_assert(0, "please define one log library at least, currently suport: USE_SPDLOG")
#endif
  }

  std::shared_ptr<LoggerImp> LoggerManager::create_logger(const std::string &logger_name, LoggerLevel level, const std::string &log_file_path, const std::string &config_file_path)
  {
    auto it = loggers_.find(logger_name);
    if (it != loggers_.end())
    {
      return it->second;
    }

#ifdef USE_SPDLOG
    auto logger = std::make_shared<SpdlogLoggerImp>(logger_name, log_file_path, config_file_path);
#else
    static_assert(0, "please define one log library at least, currently suport: USE_SPDLOG")
#endif
    logger->set_level(level);
    loggers_[logger_name] = logger;
    return logger;
  }

  std::shared_ptr<LoggerImp> LoggerManager::get_logger(const std::string &logger_name)
  {
    auto it = loggers_.find(logger_name);
    if (it != loggers_.end())
    {
      return it->second;
    }

    return nullptr;
  }

  void LoggerManager::set_logger_level(const std::string &logger_name, LoggerLevel level)
  {
    if (logger_name.empty())
    {
      default_logger_level_ = level;
      for (auto &logger : loggers_)
      {
        logger.second->set_level(level);
      }
    }
    else
    {
      auto it = loggers_.find(logger_name);
      if (it != loggers_.end())
      {
        it->second->set_level(level);
      }
    }
  }

  // define global variables
  // global logger manager
  LoggerManager &g_logger_manager = LoggerManager::get_instance();

  // default logger
  std::shared_ptr<LoggerImp> g_logger = g_logger_manager.get_default_logger();

  std::shared_ptr<LoggerImp> g_file_logger = g_logger_manager.create_logger("common", LoggerLevel::Debug, "log/major.log");
}