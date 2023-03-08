#include "client_entity.h"
#include "log/logger.h"

namespace multiplayer_server
{
  ClientEntity::ClientEntity(const std::string &id) : Entity(id)
  {
  }

  ClientEntity::~ClientEntity()
  {
  }

  void ClientEntity::update(float dt)
  {
    Entity::update(dt);
  }

  void ClientEntity::render()
  {
  }
}