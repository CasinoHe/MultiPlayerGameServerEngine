#include "server_entity.h"
#include "log/logger.h"

namespace multiplayer_server
{
  ServerEntity::ServerEntity(const std::string &id, ServerEntityType type, const std::string &ip, int port) : Entity(id), type_(type)
  {
    proxy_ = std::make_shared<EntityProxy>(g_logger);
    proxy_->set_proxy(id, ip, port);
  }

  ServerEntity::~ServerEntity()
  {
  }

  void ServerEntity::update(float dt)
  {
    Entity::update(dt);
  }

  void ServerEntity::render()
  {
    Entity::render();
  }

  // entity is a local entity if the ip and port is the same
  bool ServerEntity::is_local(const std::string &ip, int port)
  {
    // mirror entity is not a local entity
    if (ServerEntityType::kMirrorEntity == type_)
    {
      return false;
    }

    return proxy_->get_ip() == ip && proxy_->get_port() == port;
  }
}