// Created: 2023.03.10
// Author:  CasinoHe
// Purpose: network component for entity
#pragma once

#include "game/basic/component.h"
#include "network/connection.h"
#include <memory>

namespace multiplayer_server
{
  class NetworkComponent : public Component
  {
  public:
    NetworkComponent(std::shared_ptr<Connection> connection = nullptr);
    virtual ~NetworkComponent();

    // non-copyable
    NetworkComponent(const NetworkComponent&) = delete;
    NetworkComponent& operator=(const NetworkComponent&) = delete;
    NetworkComponent(NetworkComponent&&) = delete;
    NetworkComponent& operator=(NetworkComponent&&) = delete;

  public:
    // get connection
    std::shared_ptr<Connection> get_connection() const { return connection_; }
    // set connection
    void set_connection(std::shared_ptr<Connection> connection) { connection_ = connection; }

    virtual void update(float dt);
    virtual void render();

    virtual void before_destruct();

  private:
    // connection
    std::shared_ptr<Connection> connection_;
  };
}