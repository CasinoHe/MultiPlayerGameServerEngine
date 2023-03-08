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
#else
#include <format>
#endif
#include <map>
#include <variant>
#include <memory>

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
    using config_type = std::shared_ptr<std::map<std::string, std::variant<int, bool, size_t, std::string>>>;

    LoggerImp(const std::string &tag, const std::string &logfilepath = "", config_type config = nullptr) : tag_name_(tag), file_path_(logfilepath), config_(config)
    {
    }
    virtual ~LoggerImp() {}

    // Initialize logger
    virtual bool init() = 0;

    // set logger level
    virtual void set_level(LoggerLevel level) { level_ = level; } 

    // Log a message
    virtual void debug(const std::string &msg) = 0;
    virtual void info(const std::string &msg) = 0;
    virtual void warn(const std::string &msg) = 0;
    virtual void error(const std::string &msg) = 0;
    virtual void critical(const std::string &msg) = 0;

    // log a message with variadic arguments
    template <typename... Args>
    void debug(const char *fmt, const Args &... args) { log(LoggerLevel::Debug, fmt, std::forward<const Args &>(args)...); }
    template <typename... Args>
    void info(const char *fmt, const Args &... args) { log(LoggerLevel::Info, fmt, std::forward<const Args &>(args)...); }
    template <typename... Args>
    void warn(const char *fmt, const Args &... args) { log(LoggerLevel::Warn, fmt, std::forward<const Args &>(args)...); }
    template <typename... Args>
    void error(const char *fmt, const Args &... args) { log(LoggerLevel::Error, fmt, std::forward<const Args &>(args)...); }
    template <typename... Args>
    void critical(const char *fmt, const Args &... args) { log(LoggerLevel::Critical, fmt, std::forward<const Args &>(args)...); }

    // base implement of variadic argument log
    template <typename... Args>
    void log(LoggerLevel level, const char *fmt, const Args &... args)
    {
#ifdef USE_FMT
      std::string msg = fmt::format(fmt, std::forward<const Args &>(args)...);
#else
      std::string msg = std::format(fmt, std::forward<const Args &>(args)...);
#endif
      switch (level)
      {
      case LoggerLevel::Debug:
        debug(msg);
        break;
      case LoggerLevel::Info:
        info(msg);
        break;
      case LoggerLevel::Warn:
        warn(msg);
        break;
      case LoggerLevel::Error:
        error(msg);
        break;
      case LoggerLevel::Critical:
        critical(msg);
        break;
      default:
        break;
      }
    }

  protected:
    std::string tag_name_ = "major"; // Log tag
    LoggerLevel level_ = LoggerLevel::Debug;
    std::string file_path_ = ""; // Log file name
    config_type config_ = nullptr; // Log configuration 
  };
}