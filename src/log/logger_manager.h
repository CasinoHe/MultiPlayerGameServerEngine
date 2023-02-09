// Created: 2023.02.08
// Author: CasinoHe
// Purpose: Logger manager, used to manage the logger
#pragma once

#include <string>
#include <memory>
#include <map>
#include "logger_imp.h"

namespace multiplayer_server
{
  // Logger Manager is a singleton
  class LoggerManager
  {
  public:
    static LoggerManager &get_instance();

    LoggerManager();
    LoggerManager(const LoggerManager &) = delete;
    LoggerManager(LoggerManager &&) = delete;
    LoggerManager &operator=(const LoggerManager &) = delete;
    LoggerManager &operator=(LoggerManager &&) = delete;

    // Create a logger
    // The logger name is used to identify the logger
    // The logger level is used to set the logger level
    // If the logger name already exists, it will return the existing logger
    // If the logger name does not exist, it will create a logger and return it
    std::shared_ptr<LoggerImp> create_logger(const std::string &logger_name, LoggerLevel level, const std::string &log_file_path = "", const std::string &config_file_path = "");

    // Get a logger
    // The logger name is used to identify the logger
    // If the logger name does not exist, it will return nullptr
    std::shared_ptr<LoggerImp> get_logger(const std::string &logger_name);

    // Get default logger
    std::shared_ptr<LoggerImp> get_default_logger() { return default_logger_; }

    // Set logger level
    // if logger name is empty, it will set all loggers level
    void set_logger_level(const std::string &logger_name, LoggerLevel level);

  private:
    std::shared_ptr<LoggerImp> default_logger_;                 // Default logger, used when the logger name does not exist
    std::map<std::string, std::shared_ptr<LoggerImp>> loggers_; // Logger map, store all loggers
    LoggerLevel default_logger_level_ = LoggerLevel::Debug;  // Default logger level
  };
}