// Created: 2023.02.18
// Author: CasinoHe
// Purpose: Base class for all entities in the game

#pragma once
#include "log/logger.h"
#include <memory>
#include <string>
#include <map>

namespace multiplayer_server
{
  // forward declaration
  class Component;

  // abstract base class for all entities
  // define the base interface
  class Entity
  {
  public:
    Entity(const std::string &id);
    virtual ~Entity();

  // non-copyable
  private:
    Entity(const Entity&) = delete;
    Entity& operator=(const Entity&) = delete;
    Entity(const Entity&&) = delete;
    Entity& operator=(const Entity&&) = delete;

  // all interface
  public:
    virtual void update(float dt);
    virtual void render();

    // set and get name
    virtual void set_name(const std::string& name) { name_ = name; }
    virtual const std::string &get_name() { return name_; }
    // get id
    virtual const std::string &get_id() { return id_; }

    // call before destruct
    virtual void before_destruct() { delete_all_components(); }

    // get component by type 
    template<typename T>
    std::shared_ptr<T> get_component()
    {
      std::string type = typeid(T).name();
      auto it = components_.find(type);
      if (it != components_.end())
      {
        return std::static_pointer_cast<T>(it->second);
      }
      return nullptr;
    }
    // add component
    template<typename T>
    std::shared_ptr<T> add_component()
    {
      // check the type of T, must be derived from Component
      static_assert(std::is_base_of<Component, T>::value, "T must be derived from Component");

      std::string type = typeid(T).name();
      auto it = components_.find(type);
      if (it != components_.end())
      {
        return std::static_pointer_cast<T>(it->second);
      }
      std::shared_ptr<T> component = std::make_shared<T>();
      components_.emplace(type, component);
      // debug log
      logger_->debug("Entity {} add component {}", id_, type);
      return component;
    }
    // attach component
    template<typename T>
    bool attach_component(std::shared_ptr<T> component)
    {
      // check the type of T, must be derived from Component
      static_assert(std::is_base_of<Component, T>::value, "T must be derived from Component");

      std::string type = typeid(T).name();
      auto it = components_.find(type);
      if (it != components_.end())
      {
        return false;
      }
      components_.emplace(type, component);
      // log
      logger_->debug("Entity {} attach component {}", id_, type);
      return true;
    }
    // delete component
    template<typename T>
    [[nodiscard]] bool delete_component()
    {
      std::string type = typeid(T).name();
      auto it = components_.find(type);
      if (it != components_.end())
      {
        components_.erase(it);
        // debug log
        logger_->debug("Entity {} delete component {}", id_, type);
        return true;
      }

      return false;
    }
    // delete component by exists component
    template<typename T>
    [[nodiscard]] bool delete_component(std::shared_ptr<T> component)
    {
      std::string type = typeid(T).name();
      auto it = components_.find(type);
      if (it != components_.end() && it->second == component)
      {
        components_.erase(it);
        // debug log
        logger_->debug("Entity {} delete component {}", id_, type);
        return true;
      }
      return false;
    }
    // delete all components
    void delete_all_components();

  protected:
    // entity name
    std::string name_ = "";
    // entity unique id
    std::string id_ = "";

    // entity components
    std::map<std::string, std::shared_ptr<Component>> components_;
    
    // logger object
    std::shared_ptr<LoggerImp> logger_ = nullptr;
  };

  // use ip, port, entityid to identify the proxy of a  server entity
  class EntityProxy
  {
  public:
    std::string ip_ = "";
    int port_ = 0;
    std::string entity_id_ = "";

    EntityProxy(std::shared_ptr<LoggerImp> logger = g_logger) : logger_(logger) {}

    // get the string of the position
    std::string get_pos() const
    {
      return ip_ + "_" + std::to_string(port_) + "_" + entity_id_;
    }

    // parse the string to ServerEntityPos
    [[nodiscard]] bool parse_pos(const std::string &pos)
    {
      try
      {
        auto pos1 = pos.find("_");
        auto pos2 = pos.rfind("_");
        ip_ = pos.substr(0, pos1);
        port_ = std::stoi(pos.substr(pos1 + 1, pos2 - pos1 - 1));
        entity_id_ = pos.substr(pos2 + 1);
        validate = true;
        return true;
      }
      catch (const std::exception &e)
      {
        logger_->error("ServerEntityPos::parse_pos error: {}", e.what());
        return false;
      }
    }

    void set_proxy(const std::string &id, const std::string &ip, int port)
    {
      this->entity_id_ = id;
      this->ip_ = ip;
      this->port_ = port;
      validate = true;
    }

    // get valid
    bool is_valid() const { return validate; }

  private:
    std::shared_ptr<LoggerImp> logger_ = nullptr;
    bool validate = false;        // if the pos is valid
  };

  class ServerEntity : public Entity
  {
  public:
    // there are many kinds of server entities
    // for example: 
    //  1. mirror entity, which is a copy of server entity in other server. it doesn't have any logic or save any data, but it can forward the message to the server entity
    //  2. player entity, which can conmucate with client entities
    //  3. service entity, which is a real entity in the server and provide some service to other entities
    //  4. AI entity, the difference between player entity is that AI entity doesn't have a real client entity, server controls the behavior of AI entity
    //  4. freely combination of components, for example, a AI Entity can have a controller component, so it can be controlled by a player entity
    //  etc.
    // so we need to define a enum to distinguish them
    enum class ServerEntityType
    {
      kMirrorEntity,
      kPlayerEntity,
      kServiceEntity,
      kAIEntity,
      kFreelyCombinedEntity,
    };

  public:
    ServerEntity(const std::string &id, ServerEntityType type, const std::string &ip, int port);
    virtual ~ServerEntity();

    virtual void update(float dt) override;
    virtual void render() override;

    // get server type
    const ServerEntityType get_server_type() const { return type_; }
    // get server proxy
    const EntityProxy &get_proxy() const { return proxy_; }

  protected:
    // server proxy, which server this entity belongs to
    EntityProxy proxy_;

    // server entity type
    ServerEntityType type_ = ServerEntityType::kServiceEntity;
  };

  class ClientEntity : public Entity
  {
  public:
    ClientEntity(const std::string &id);
    virtual ~ClientEntity();

    virtual void update(float dt) override;
    virtual void render() override;
  };
}