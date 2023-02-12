// Created: 2023.02.08
// Author: CasinoHe
// Desc: In order to ensure the scalability of the log module, 
//     the underlying implementation can be switched. Here is a class that is used in the upper layer.
//     As for the implementation of the bottom layer, it can be SPDLOG, NONO, LOG4CPLUS, GLOG, or other log libraries.
//     the upper layer only needs to implement the interface as simple as possible.
#pragma once

#include <string>
#include <stdexcept>
#ifdef USE_FMT
#include <fmt/color.h>
#include <fmt/chrono.h>
#include <fmt/ranges.h>
#endif

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
    LoggerImp(const std::string &tag, const std::string &logfilepath = "", const std::string &config_path = "") : tag_name_(tag), file_path_(logfilepath)
    {
    }
    ~LoggerImp() {}

    // Initialize logger
    virtual bool init(const std::string &config_file_path) = 0;

    // set logger level
    virtual void set_level(LoggerLevel level) { level_ = level; } 

    // Log a message
    virtual void debug(const std::string &msg) = 0;
    virtual void log(const std::string &msg) = 0;
    virtual void info(const std::string &msg) = 0;
    virtual void warn(const std::string &msg) = 0;
    virtual void error(const std::string &msg) = 0;
    virtual void critical(const std::string &msg) = 0;

    // log a message with variadic arguments
    template <typename... Args>
    void debug(const char *fmt, const Args &... args) { throw std::runtime_error("not implemented"); }
    template <typename... Args>
    void log(const char *fmt, const Args &... args) { throw std::runtime_error("not implemented"); }
    template <typename... Args>
    void info(const char *fmt, const Args &... args) { throw std::runtime_error("not implemented"); }
    template <typename... Args>
    void warn(const char *fmt, const Args &... args) { throw std::runtime_error("not implemented"); }
    template <typename... Args>
    void error(const char *fmt, const Args &... args) { throw std::runtime_error("not implemented"); }
    template <typename... Args>
    void critical(const char *fmt, const Args &... args) { throw std::runtime_error("not implemented"); }



  protected:
    std::string tag_name_ = "major"; // Log tag
    LoggerLevel level_ = LoggerLevel::Debug;
    std::string file_path_ = ""; // Log file name
  };
}