#include "component.h"

namespace multiplayer_server
{
  Component::Component(std::shared_ptr<Entity> owner) : owner_(owner)
  {
  }

  Component::~Component()
  {
  }

  // get owner, return a shared_ptr
  // because the owner_ is a weak_ptr, so we need to lock it to get a shared_ptr
  std::shared_ptr<Entity> Component::get_owner()
  {
    // first, check if the owner_ is expired
    if (owner_.expired())
    {
      return nullptr;
    }
    else
    {
      return owner_.lock();
    }
  }
}