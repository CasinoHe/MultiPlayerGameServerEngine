#include "entity.h"
#include "component.h"

namespace multiplayer_server
{
  Entity::Entity(const std::string& id)
    : id_(id)
  {
    components_.clear();
    // create logger
    logger_ = g_logger_manager.create_logger("Entity", LoggerLevel::Debug, "log/entity.log");
    logger_->debug("Entity {} constructed", id_);
  }

  Entity::~Entity()
  {
    logger_->debug("Entity {} destructed", id_);

    // release all components
    delete_all_components();
  }

  // delete all components
  void Entity::delete_all_components()
  {
    for (auto it = components_.begin(); it != components_.end(); ++it)
    {
      it->second->before_destruct();
    }
    components_.clear();
  }

  // logic update
  void Entity::update(float dt)
  {
    for (auto it = components_.begin(); it != components_.end(); ++it)
    {
      it->second->update(dt);
    }
  }

  // render update
  void Entity::render()
  {
    for (auto it = components_.begin(); it != components_.end(); ++it)
    {
      it->second->render();
    }
  }

  ServerEntity::ServerEntity(const std::string &id) : Entity(id), pos_(logger_)
  {
    pos_.entity_id = id_;
    pos_.ip = "";
    pos_.port = 0;
  }

  ServerEntity::~ServerEntity()
  {
  }

  void ServerEntity::update(float dt)
  {
  }

  void ServerEntity::render()
  {
  }

  ClientEntity::ClientEntity(const std::string &id) : Entity(id)
  {
  }

  ClientEntity::~ClientEntity()
  {
  }

  void ClientEntity::update(float dt)
  {
  }

  void ClientEntity::render()
  {
  }
}