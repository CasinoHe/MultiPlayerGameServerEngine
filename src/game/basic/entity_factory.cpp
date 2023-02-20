#include "entity_factory.h"
#include "entity.h"
#include "uuid.h"

namespace multiplayer_server
{
  // global access for entity factory instance
  EntityFactory& g_entity_factory = EntityFactory::get_instance();

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
}