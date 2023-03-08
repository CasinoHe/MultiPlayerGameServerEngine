#include "login_service.h"
#include "network/connection.h"
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
    ServerEntity::update(dt);
  }

  void LoginService::render()
  {
  }

  // implement asio tcp connection handler
  bool LoginService::on_client_connected(std::shared_ptr<Connection> connection)
  {
    auto conn = connection;
    logger_->debug("LoginService::on_client_connected");
    return true;
  }
}