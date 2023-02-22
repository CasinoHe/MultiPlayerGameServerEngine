// Created: 2023.02.22
// Author: CasinoHe
// Purpose: record global services
#pragma once
#include <map>
#include <string>
#include <memory>

namespace multiplayer_server
{
  class ServerEntity;

  // global game interfaces and data
  class GameMain
  {
  public:
    GameMain(const std::string &ip, int port);
    ~GameMain();

    // non-copyable
    GameMain(const GameMain&) = delete;
    GameMain& operator=(const GameMain&) = delete;
    GameMain(GameMain&&) = delete;
    GameMain& operator=(GameMain&&) = delete;

  public:
    // init game service
    void init_game_service(const std::string &name);
    // get game service
    std::shared_ptr<ServerEntity> get_game_service(const std::string &service_name);

  private:
    // all game services
    std::map<std::string, std::shared_ptr<ServerEntity>> game_services_;
    // save ip and port
    std::string ip_;
    int port_;
  };
}