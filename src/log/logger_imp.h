// Created: 2023.02.08
// Author: CasinoHe
// Desc: In order to ensure the scalability of the log module, 
//     the underlying implementation can be switched. Here is a class that is used in the upper layer.
//     As for the implementation of the bottom layer, it can be SPDLOG, NONO, LOG4CPLUS, GLOG, or other log libraries.
//     the upper layer only needs to implement the interface as simple as possible.
#pragma once

#include <string>

namespace multiplayer_server
{
  // all log levels
  enum class LoggerLevel
  {
    Debug,
    Info,
    Warn,
    Error,
    Critical,
  };

  class LoggerImp
  {
  public:
    LoggerImp();
    ~LoggerImp();

    // Initialize logger
    virtual bool init(const std::string &config_file_path) = 0;

    // set logger level
    void set_level(LoggerLevel level) { level_ = level; }

    // Log a message
    virtual void log(const std::string &msg) = 0;

    // Log a message with level
    virtual void log(const std::string &msg, const std::string &level) = 0;

    // Log a message with level and file name
    virtual void log(const std::string &msg, const std::string &level, const std::string &file_name) = 0;

    // Log a message with level, file name and line number
    virtual void log(const std::string &msg, const std::string &level, const std::string &file_name, int line_number) = 0;

    // Log a message with level, file name, line number and function name
    virtual void log(const std::string &msg, const std::string &level, const std::string &file_name, int line_number, const std::string &function_name) = 0;

  private:
    LoggerLevel level_;
  };
}