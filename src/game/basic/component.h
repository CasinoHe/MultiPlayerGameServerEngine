// Created: 2023.02.18
// Author: CasinoHe
// Purpose: Base class for all components in the game
#pragma once
#include <string>

namespace multiplayer_server
{
  class Component
  {
  public:
    Component();
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

  protected:
    std::string name_ = "";
  };
}