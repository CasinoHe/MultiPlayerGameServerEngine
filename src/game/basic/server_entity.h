// Created: 2023.03.08
// Author: CasinoHe
// Purpose: implement a abstract base server entity class for all server entities

#pragma once

#include "game/basic/entity.h"

namespace multiplayer_server
{
  class ServerEntity : public Entity
  {
  public:
    // there are many kinds of server entities
    // for example: 
    //  1. mirror entity, which is a copy of server entity in other server. it doesn't have any logic or save any data, but it can forward the message to the server entity
    //  2. player entity, which can conmucate with client entities
    //  3. service entity, which is a real entity in the server and provide some service to other entities
    //  4. AI entity, the difference between player entity is that AI entity doesn't have a real client entity, server controls the behavior of AI entity
    //  4. freely combination of components, for example, a AI Entity can have a controller component, so it can be controlled by a player entity
    //  etc.
    // so we need to define a enum to distinguish them
    enum class ServerEntityType
    {
      kMirrorEntity,
      kPlayerEntity,
      kServiceEntity,
      kAIEntity,
      kFreelyCombinedEntity,
    };

  public:
    ServerEntity(const std::string &id, ServerEntityType type, const std::string &ip, int port);
    virtual ~ServerEntity();

    virtual void update(float dt) override;
    virtual void render() override;

    // get server type
    ServerEntityType get_server_type() const { return type_; }
    // get server proxy
    const std::shared_ptr<EntityProxy> &get_proxy() const { return proxy_; }

    // if this entity is a local entity
    bool is_local(const std::string &ip, int port);

    // get entity type from string
    static ServerEntityType get_type_from_string(const std::string &type);

  protected:
    // server entity type
    ServerEntityType type_ = ServerEntityType::kServiceEntity;

    // proxy of the entity, use shared_ptr to avoid the entity destruct before the proxy
    std::shared_ptr<EntityProxy> proxy_ = nullptr;
  };
}