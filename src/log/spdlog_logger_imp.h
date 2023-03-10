// Created: 2023.02.08
// Author: CasinoHe
// Purpose: bottom layer implementation of spdlog
#pragma once

#include "logger_imp.h"
#include "spdlog/spdlog.h"       // include submodule spdlog/spdlog.h
#include <string>
#include <map>
#include <memory>

namespace multiplayer_server
{
  class SpdlogLoggerImp : public LoggerImp
  {
  public:
    SpdlogLoggerImp(const std::string &logger_name, const std::string& logpath = "", LoggerImp::config_type config = nullptr);
    virtual ~SpdlogLoggerImp();

    // Initialize logger
    bool init() override;
    bool read_config();

    // set log level
    void set_level(LoggerLevel level) override;

    // Log a message
    void debug(const std::string &msg) override;
    void info(const std::string &msg) override;
    void warn(const std::string &msg) override;
    void error(const std::string &msg) override;
    void critical(const std::string &msg) override;

  private:
    bool need_log_time_ = true; // Whether to log time
    bool need_log_thread_ = true; // Whether to log thread
    bool need_log_level_ = true; // Whether to log level
    bool need_log_source_ = false; // Whether to log source file name
    bool need_log_line_ = false; // Whether to log line number
    bool need_log_funcname_ = false; // Whether to log function name

    size_t log_file_size_ = 0; // Log file size
    size_t log_file_cnt_ = 0; // Log file count
    std::shared_ptr<spdlog::logger> logger_ = nullptr; // Log object
  };
}