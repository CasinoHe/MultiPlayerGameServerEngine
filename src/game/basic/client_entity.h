// Created: 2023.03.08
// Author: CasinoHe
// Purpose: implement a abstract base server entity class for all client entities

#pragma once

#include "game/basic/entity.h"

namespace multiplayer_server
{
  class ClientEntity : public Entity
  {
  public:
    ClientEntity(const std::string &id);
    virtual ~ClientEntity();

    virtual void update(float dt) override;
    virtual void render() override;
  };
}