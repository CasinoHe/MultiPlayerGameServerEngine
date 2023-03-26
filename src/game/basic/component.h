// Created: 2023.02.18
// Author: CasinoHe
// Purpose: Base class for all components in the game
#pragma once
#include <string>
#include <memory>

namespace multiplayer_server
{
  // forward declaration, Component need to know Entity who is the owner of it
  class Entity;

  class Component
  {
  public:
    Component(std::shared_ptr<Entity> owner);
    virtual ~Component();

  // non-copyable
  private:
    Component(const Component&) = delete;
    Component& operator=(const Component&) = delete;
    Component(const Component&&) = delete;
    Component& operator=(const Component&&) = delete;

  // all interface
  public:
    virtual void update(float dt) = 0;
    virtual void render() = 0;

    // set and get name
    virtual void set_name(const std::string& name) { name_ = name; }
    virtual const std::string &get_name() { return name_; }

    // before destruct
    virtual void before_destruct() = 0;

    // get owner, return a shared_ptr
    virtual std::shared_ptr<Entity> get_owner();
    virtual void set_owner(std::shared_ptr<Entity> owner) { owner_ = owner; }
    virtual void reset_owner() { owner_.reset(); }

  protected:
    std::string name_ = "";
    // owner of this component, use weak_ptr to avoid circular reference
    std::weak_ptr<Entity> owner_;
  };
}