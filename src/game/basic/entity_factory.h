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
  class EntityFactory;
  class UUID;

  // global access for entity factory instance
  extern EntityFactory& g_entity_factory;

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
    template <typename T>
    std::shared_ptr<T> create_entity()
    {
      // check the type of T, must be derived from Entity
      static_assert(std::is_base_of<Entity, T>::value, "T must be derived from Entity");

      std::shared_ptr<T> entity = std::make_shared<T>((*uuid_)());
      entities_.emplace(entity->get_id(), entity);
      
      // TODO: register entity to a global entity manager, especially for a huge number of server processes
      return entity;
    }
    // template function, create a new entity with id and template type
    template <typename T>
    std::shared_ptr<T> create_entity(const std::string& id)
    {
      // check the type of T, must be derived from Entity
      static_assert(std::is_base_of<Entity, T>::value, "T must be derived from Entity");

      std::shared_ptr<T> entity = std::make_shared<T>(id);
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