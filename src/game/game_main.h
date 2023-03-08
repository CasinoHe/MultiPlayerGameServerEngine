// Created: 2023.02.22
// Author: CasinoHe
// Purpose: record global services
#pragma once

#include "game/basic/entity_factory.h"
#include <map>
#include <string>
#include <memory>
#include <functional>
#include <tuple>

namespace multiplayer_server
{
  class ServerEntity;
  class Connection;
  class GameConfig;

  // global game interfaces and data
  class GameMain
  {
  public:
    GameMain(const std::string &config_file_path);
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
    bool on_client_connected(std::shared_ptr<Connection> connection);

    // return game ip and port
    std::string get_ip() const { return ip_; }
    int get_port() const { return port_; }
    // return ip and port as tuple
    std::tuple<std::string, int> get_ip_port() const { return std::make_tuple(ip_, port_); }

    // create a new login entity using the data read from config file
    std::shared_ptr<ServerEntity> create_login_entity();

  private:
    // init a game service
    void init_game_service(const std::string &name);

  private:
    // all game services
    std::map<std::string, std::shared_ptr<ServerEntity>> game_services_;

    // save ip and port
    std::string ip_ = "127.0.0.1";
    int port_ = 8080;

    // game config file parser
    std::shared_ptr<GameConfig> game_config_;

    // game entity factory
    EntityFactory& entity_factory_ = EntityFactory::get_instance();
  };
}