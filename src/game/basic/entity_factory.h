// Created: 2023.02.18
// Author: CasinoHe
// Purpose: entity factory, all entities need to be created by this factory

#pragma once

#include <memory>
#include <map>
#include <string>

namespace multiplayer_server
{
  // forward declaration for entity
  class Entity;
  class UUID;

  // entity factory is a singleton
  // final class
  class EntityFactory final
  {
  public:
    ~EntityFactory();

  public:
    // get the singleton instance
    static EntityFactory& get_instance();

    // get entity by id
    std::shared_ptr<Entity> get_entity(const std::string& id);

    // template function, create a new entity with template type
    template <typename T, typename ...Args>
    std::shared_ptr<T> create_entity(Args... args)
    {
      // check the type of T, must be derived from Entity
      static_assert(std::is_base_of<Entity, T>::value, "T must be derived from Entity");

      // create entity and forward the args
      std::shared_ptr<T> entity = std::make_shared<T>(std::forward<Args>(args)...);
      entities_.emplace(entity->get_id(), entity);

      // TODO: register entity to a global entity manager, especially for a huge number of server processes
      return entity;
    }

    // template function, create a new entity with id and template type
    template <typename T, typename ...Args>
    std::shared_ptr<T> create_entity_with_id(const std::string& id, Args... args)
    {
      // check the type of T, must be derived from Entity
      static_assert(std::is_base_of<Entity, T>::value, "T must be derived from Entity");

      std::shared_ptr<T> entity = std::make_shared<T>(id, std::forward<Args>(args)...);
      entities_.emplace(id, entity);

      // TODO: register entity to a global entity manager, especially for a huge number of server processes
      return entity;
    }

  // non-copyable
  private:
    EntityFactory();
    EntityFactory(const EntityFactory&) = delete;
    EntityFactory& operator=(const EntityFactory&) = delete;
    EntityFactory(const EntityFactory&&) = delete;
    EntityFactory& operator=(const EntityFactory&&) = delete;

  private:
    // map of all entities
    std::map<std::string, std::shared_ptr<Entity>> entities_;
    std::shared_ptr<UUID> uuid_;
  };
}