#include "logger_manager.h"

namespace multiplayer_server
{
  LoggerManager &LoggerManager::get_instance()
  {
    static LoggerManager instance;
    return instance;
  }

  LoggerManager::LoggerManager()
  {
    default_logger_ = std::make_shared<LoggerImp>();
  }

  std::shared_ptr<LoggerImp> LoggerManager::create_logger(const std::string &logger_name, LoggerLevel level)
  {
    auto it = loggers_.find(logger_name);
    if (it != loggers_.end())
    {
      return it->second;
    }

    auto logger = std::make_shared<LoggerImp>();
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

    return default_logger_;
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
}