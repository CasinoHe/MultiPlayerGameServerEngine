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
      auto entity_type = ServerEntityType::kPlayerEntity;
      if (data_ptr->entity_type == "kMirrorEntity")
      {
        entity_type = ServerEntityType::kMirrorEntity;
      }
      else if (data_ptr->entity_type == "kServiceEntity")
      {
        entity_type = ServerEntityType::kServiceEntity;
      }
      else if (data_ptr->entity_type == "kPlayerEntity")
      {
        entity_type = ServerEntityType::kPlayerEntity;
      }
      else if (data_ptr->entity_type == "kAIEntity")
      {
        entity_type = ServerEntityType::kAIEntity;
      }
      else if (data_ptr->entity_type == "kFreelyCombinedEntity")
      {
        entity_type = ServerEntityType::kFreelyCombinedEntity;
      }
      else
      {
        logger_->error("LoginService::on_client_connected, can not find entity type: {}", data_ptr->entity_type);
        return false;
      }

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
      }
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
    return;
  }
}