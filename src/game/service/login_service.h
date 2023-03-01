// Created: 2023.02.22
// Author: CasinoHe
// Purpose: implement login service on a game server
#pragma once

#include "game/basic/entity.h"

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

    // since there are varities of connection type, we need to implement callback using temlate
    template <typename T>
    bool on_client_connected(std::shared_ptr<T> connection)
    {
      logger_->debug("LoginService::on_client_connected");
      return true;
    }
  };
}