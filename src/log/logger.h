// Created: 2023.01.08
// Author: CasinoHe
// Purpose: log include file

#pragma once

#include "logger_manager.h"

// global logger manager
multiplayer_server::LoggerManager &g_logger_manager = multiplayer_server::LoggerManager::get_instance();