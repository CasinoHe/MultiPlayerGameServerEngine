#include "network_component.h"
#include "game/basic/entity.h"

namespace multiplayer_server
{
  NetworkComponent::NetworkComponent(std::shared_ptr<Entity> owner, std::shared_ptr<Connection> connection) : 
    Component(owner), connection_(connection)
  {
    set_name("NetworkComponent");
  }

  NetworkComponent::~NetworkComponent()
  {
  }

  void NetworkComponent::update([[maybe_unused]]float dt)
  {
  }

  void NetworkComponent::render()
  {
  }

  // abstract method for before destruct
  void NetworkComponent::before_destruct()
  {
  }

  void NetworkComponent::handle_disconnect()
  {
    // if owner is nullptr, return
    auto owner = get_owner();

    if (!owner)
    {
      return;
    }

    for (auto &handler : disconnect_handlers_)
    {
      const std::string &id = owner->get_id();
      handler.second(id);
    }
  }

  // register disconnect handler
  bool NetworkComponent::register_disconnect_handler(const std::string &name, std::function<void(const std::string &id)> handler)
  {
    if (disconnect_handlers_.find(name) != disconnect_handlers_.end())
    {
      return false;
    }

    disconnect_handlers_[name] = handler;
    return true;
  }
}