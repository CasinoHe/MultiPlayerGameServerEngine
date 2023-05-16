#include "game_main.h"
#include "game/basic/entity.h"
#include "game/basic/entity_factory.h"
#include "game/service/login_service.h"
#include "config/game_config.h"
#include <tuple>

namespace multiplayer_server
{
  GameMain::GameMain(const std::string &config_file_path)
  {
    // get config value, especially for ip and port
    game_config_ = std::make_unique<GameConfig>(config_file_path);

    auto ptr = game_config_->get_server_ip_port();
    if (!ptr)
    {
      return;
    }

    ip_ = std::get<0>(*ptr);
    port_ = std::get<1>(*ptr);
  }

  GameMain::~GameMain()
  {
  }

  void GameMain::init_game_service(const std::string &name)
  {
    std::shared_ptr<ServerEntity> game_service = nullptr;

    if (name == "LoginService")
    {
      // create a login service
      std::shared_ptr<LoginService> login_service = entity_factory_.create_entity<LoginService>(ip_, port_, game_config_);
      game_service = std::dynamic_pointer_cast<ServerEntity>(login_service);
    }

    if (game_service)
    {
      // add to game services
      record_game_service(name, game_service);
    }
  }

  std::shared_ptr<ServerEntity> GameMain::get_game_service(const std::string &service_name)
  {
    auto iter = game_services_.find(service_name);
    if (iter != game_services_.end())
    {
      return iter->second.front();
    }
    return nullptr;
  }

  // get game service by id
  std::shared_ptr<ServerEntity> GameMain::get_game_service(const std::string &service_name, const std::string &id)
  {
    auto iter = game_services_.find(service_name);
    if (iter != game_services_.end())
    {
      for (auto &service : iter->second)
      {
        if (service->get_id() == id)
        {
          return service;
        }
      }
    }
    return nullptr;
  }

  // get local game service
  std::shared_ptr<ServerEntity> GameMain::get_local_game_service(const std::string &service_name)
  {
    auto iter = game_services_.find(service_name);
    if (iter != game_services_.end())
    {
      for (auto &service : iter->second)
      {
        if (service->is_local(ip_, port_))
        {
          return service;
        }
      }
    }
    return nullptr;
  }

  void GameMain::init_all_game_services()
  {
    // first, get all services that need to be init from game config

    init_game_service("LoginService");

    // then, read config file to init other services
  }

  bool GameMain::on_client_connected(std::shared_ptr<Connection> connection)
  {
    // get login service
    std::shared_ptr<LoginService> login_service = std::dynamic_pointer_cast<LoginService>(get_game_service("LoginService"));
    if (login_service)
    {
      return login_service->on_client_connected(connection);
    }
    return false;
  }

  // record game service
  void GameMain::record_game_service(const std::string& name, std::shared_ptr<ServerEntity> game_service)
  {
    auto [iter, inserted] = game_services_.insert_or_assign(name, std::list<std::shared_ptr<ServerEntity>>());
    iter->second.emplace_back(std::move(game_service));
  }
}