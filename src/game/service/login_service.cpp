#include "login_service.h"
#include "log/logger.h"

namespace multiplayer_server
{
  LoginService::LoginService(const std::string &ip, int port) : ServerEntity("LoginService", ServerEntityType::kServiceEntity, ip, port)
  {
  }

  LoginService::~LoginService()
  {
  }

  void LoginService::update(float dt)
  {
  }

  void LoginService::render()
  {
  }
}