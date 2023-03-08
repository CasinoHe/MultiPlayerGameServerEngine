#include "server_entity.h"
#include "log/logger.h"

namespace multiplayer_server
{
  ServerEntity::ServerEntity(const std::string &id, ServerEntityType type, const std::string &ip, int port) : Entity(id), type_(type)
  {
    proxy_.set_proxy(id, ip, port);
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
}