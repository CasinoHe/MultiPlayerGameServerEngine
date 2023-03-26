// Created: 2023.02.22
// Author: CasinoHe
// Purpose: implement login service on a game server
#pragma once

#include "game/basic/server_entity.h"
#include "game/basic/entity_factory.h"
#include <memory>

namespace multiplayer_server
{
  class Connection;
  class GameConfig;

  // this is final class
  class LoginService final: public ServerEntity
  {
  public:
    LoginService(const std::string &id, const std::string &ip, int port, std::shared_ptr<GameConfig> game_config);
    virtual ~LoginService();

    void update(float dt) override;
    void render() override;

    // In spite of the fact that there are varities of connection type
    // game only need to know the connection is connected and use the abstract connection type
    bool on_client_connected(std::shared_ptr<Connection> connection);

    // game only need to know the connection is disconnected
    void on_client_disconnected(const std::string &id);

  private:
    // game config
    std::shared_ptr<GameConfig> game_config_;
    // entity factory
    EntityFactory &entity_factory_ = EntityFactory::get_instance();
  };
}