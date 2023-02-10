// Created: 2023.02.08
// Author: CasinoHe
// Purpose: log include file
#pragma once

#include "logger_manager.h"
#include <memory>

namespace multiplayer_server
{
  // variable declaration, see logger_manager.cpp
  // global logger manager
  extern LoggerManager &g_logger_manager;

  // default logger
  extern std::shared_ptr<LoggerImp> g_logger;
}