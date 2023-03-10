#include "network_component.h"

namespace multiplayer_server
{
  NetworkComponent::NetworkComponent(std::shared_ptr<Connection> connection) : 
    connection_(connection)
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
}