#include "entity.h"
#include "component.h"
#include "game/component/network_component.h"

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

    // call the before destruct function
    before_destruct();
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

  // init component from config using std::vector<std::string> as name list
  void Entity::init_components(const std::vector<std::string>& names)
  {
    for (auto it = names.begin(); it != names.end(); ++it)
    {
      std::string name = *it;
      if (name == "NetworkComponent")
      {
        add_component<NetworkComponent>();
      }
    }
  }

  // update owner of component
  void Entity::update_component_owner(std::shared_ptr<Component> component, std::shared_ptr<Entity> owner)
  {
    if (!owner)
    {
      component->reset_owner();
    }
    else
    {
      component->set_owner(owner);
    }
  }
}