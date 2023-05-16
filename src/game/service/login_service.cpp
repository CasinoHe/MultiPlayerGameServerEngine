#include "login_service.h"
#include "network/connection.h"
#include "log/logger.h"
#include "config/game_config.h"
#include "game/component/network_component.h"

namespace multiplayer_server
{
  LoginService::LoginService(const std::string &id, const std::string &ip, int port, std::shared_ptr<GameConfig> game_config) : 
    ServerEntity(id, ServerEntityType::kServiceEntity, ip, port),
    game_config_(game_config)
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
    logger_->debug("LoginService::on_client_connected");

    // create entity for this connection use configuration data from game config
    // first, find witch entity class should be created
    auto data_ptr = game_config_->get<LoginEntityConfig>(LOGIN_CONFIG_STR);
    if (!data_ptr)
    {
      logger_->error("LoginService::on_client_connected, can not find login entity config data");
      return false;
    }
    auto entity_class_name = data_ptr->entity_class_name;

    // second, create entity
    if (entity_class_name == "ServerEntity")
    {
      auto entity_type = ServerEntity::get_type_from_string(data_ptr->entity_type);

      auto entity = entity_factory_.create_entity<ServerEntity>(entity_type, proxy_->get_ip(), proxy_->get_port());

      if (data_ptr->entity_components.size() > 0)
      {
        entity->init_components(data_ptr->entity_components);
      }

      // get network component and set connection
      auto network_component = entity->get_component<NetworkComponent>();
      if (network_component)
      {
        network_component->set_connection(connection);
        network_component->register_disconnect_handler("LoginService", std::bind(&LoginService::on_client_disconnected, this, std::placeholders::_1));
      }

      // TODO:
      // 1. authorize the entity
      // 2. check if the entity is already in the game or already in global entity manager
      // 3. register entity to global entity manager so that any other entity can find this entity by id wherever it is and which process it is
    }

    return true;
  }

  void LoginService::on_client_disconnected(const std::string &id)
  {
    auto entity = entity_factory_.get_entity(id);
    if (!entity)
    {
      return;
    }

    logger_->debug("LoginService::on_client_disconnected");

    // TODO:
    // unregister entity from global entity manager

    return;
  }
}