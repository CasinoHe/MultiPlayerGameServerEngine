// Created: 2023.02.22
// Author: CasinoHe
// Purpose: implement login service on a game server
#pragma once

#include "game/basic/entity.h"

namespace multiplayer_server
{
  class LoginService : public ServerEntity
  {
  public:
    LoginService(const std::string &ip, int port);
    virtual ~LoginService();

    virtual void update(float dt) override;
    virtual void render() override;
  };
}