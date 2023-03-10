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
}