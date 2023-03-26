#include "entity_factory.h"
#include "entity.h"
#include "uuid.h"

namespace multiplayer_server
{
  EntityFactory::EntityFactory()
  {
    entities_.clear();
    uuid_ = std::make_shared<UUID>();
  }

  EntityFactory::~EntityFactory()
  {
  }

  // get the singleton instance
  EntityFactory& EntityFactory::get_instance()
  {
    static EntityFactory instance;
    return instance;
  }

  // generate new unique id
  std::string EntityFactory::generate_id()
  {
    return uuid_->get();
  }

  // get entity by id
  std::shared_ptr<Entity> EntityFactory::get_entity(const std::string& id)
  {
    auto iter = entities_.find(id);
    if (iter != entities_.end())
    {
      return iter->second;
    }
    return nullptr;
  }

  // destroy entity by id
  void EntityFactory::destroy_entity(const std::string& id)
  {
    auto iter = entities_.find(id);
    if (iter != entities_.end())
    {
      entities_.erase(iter);
    }

    // we need to check the reference count of the entity
    // if the reference count is 0, we can delete the entity
    // if the reference count is not 0, we can't delete the entity
    // because there are some other shared_ptr point to the entity
    // so we need to set the entity to be invalid
    if (iter->second.use_count() == 1)
    {
      // use count is 1, means only iter->second owns the entity 
      // so we can delete the entity
      iter->second.reset();
    }
    else if (iter->second.use_count() > 1)
    {
      // use count is greater than 1, means there are some other shared_ptr point to the entity
      // so we can't release the shared_ptr, or we cannot obtain the entity
      // we need to set the entity to be invalid
      iter->second->set_validate(false);
    }
    else
    {
      // use count is 0, means the entity is invalid
      // we don't need to do anything
      // TODO: maybe there is an error
    }
  }
}