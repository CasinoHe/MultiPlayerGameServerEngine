#include "game_main.h"
#include "game/basic/entity.h"
#include "game/basic/entity_factory.h"
#include "game/service/login_service.h"

namespace multiplayer_server
{
  GameMain::GameMain(const std::string &ip, int port) : ip_(ip), port_(port)
  {
  }

  GameMain::~GameMain()
  {
  }

  void GameMain::init_game_service(const std::string &name)
  {
    if (name == "LoginService")
    {
      // create a login service
      std::shared_ptr<LoginService> login_service = g_entity_factory.create_entity<LoginService>(ip_, port_);
    }
  }

  std::shared_ptr<ServerEntity> GameMain::get_game_service(const std::string &service_name)
  {
    auto iter = game_services_.find(service_name);
    if (iter != game_services_.end())
    {
      return iter->second;
    }
    return nullptr;
  }
}