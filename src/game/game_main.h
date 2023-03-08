// Created: 2023.02.22
// Author: CasinoHe
// Purpose: record global services
#include "game/service/login_service.h"
#pragma once
#include <map>
#include <string>
#include <memory>
#include <functional>

namespace multiplayer_server
{
  class ServerEntity;
  class Connection;

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
    // init all game services
    void init_all_game_services();

    // get game service
    std::shared_ptr<ServerEntity> get_game_service(const std::string &service_name);

    // In spide of the fact that there are varities of connection type
    // game only need to know the connection is connected and use the abstract connection type
    bool on_client_connected(std::shared_ptr<Connection> connection)
    {
      // get login service
      std::shared_ptr<LoginService> login_service = std::dynamic_pointer_cast<LoginService>(get_game_service("LoginService"));
      if (login_service)
      {
        return login_service->on_client_connected(connection);
      }
      return false;
    }

  private:
    // init a game service
    void init_game_service(const std::string &name);

  private:
    // all game services
    std::map<std::string, std::shared_ptr<ServerEntity>> game_services_;
    // save ip and port
    std::string ip_;
    int port_;
  };
}