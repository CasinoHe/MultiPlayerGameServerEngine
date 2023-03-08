// Created: 2023.02.22
// Author: CasinoHe
// Purpose: implement login service on a game server
#pragma once

#include "game/basic/server_entity.h"

namespace multiplayer_server
{
  class Connection;

  // this is final class
  class LoginService final: public ServerEntity
  {
  public:
    LoginService(const std::string &ip, int port);
    virtual ~LoginService();

    void update(float dt) override;
    void render() override;

    // In spite of the fact that there are varities of connection type
    // game only need to know the connection is connected and use the abstract connection type
    bool on_client_connected(std::shared_ptr<Connection> connection);
  };
}