#include "config/arg_parser.h"
#include "log/logger.h"
#include "network/asio_server.h"
#include "game/game_main.h"
#include <iostream>
#include <filesystem>
#include <chrono>
#include <thread>
#include <functional>

// except g_logger and g_logger_manager, there is no global instance
// all other game objects are created in game_main object, Reason:
// 1. avoid using global instance, it is not thread safe
// 2. reduce the difficulty of maintaining the code, especially when we need init some objects in a specific order
int check_config_file(const std::string &config_file_path)
{
  using namespace multiplayer_server;

  // no config file path, exit with error
  if (config_file_path.empty())
  {
    g_logger->error("config file path is empty");
    return EXIT_FAILURE;
  }
  // config file is not exist, exit with error
  if (!std::filesystem::exists(config_file_path))
  {
    g_logger->error("config file is not exist, path: {}", config_file_path);
    return EXIT_FAILURE;
  }
  // config file path is not a file, exit with error
  if (!std::filesystem::is_regular_file(config_file_path))
  {
    g_logger->error("config file path is not a file, path: {}", config_file_path);
    return EXIT_FAILURE;
  }
  // config file path is not a json file, exit with error
  if (config_file_path.substr(config_file_path.find_last_of('.')) != ".json")
  {
    g_logger->error("config file path is not a json file, path: {}", config_file_path);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int main(int argc, const char **argv)
{
  using namespace multiplayer_server;

  ArgParser arg_parser(argc, argv);

  // if parse failed, exit
  if (!arg_parser.parse())
  {
    return EXIT_FAILURE;
  }

  // check config file
  const std::string &config_file_path = arg_parser.get_config_file_path();
  if (check_config_file(config_file_path) != EXIT_SUCCESS)
  {
    return EXIT_FAILURE;
  }

  // create game_main object, init all services
  auto game_main = std::make_shared<GameMain>(config_file_path);
  game_main->init_all_game_services();

  // create asio server
  const auto [ip, port] = game_main->get_ip_port();
  auto asio_server = std::make_unique<AsioServer>(ip, port, true, false);
  asio_server->set_io_context_thread_count(10);

  // register connected callback
  std::function<bool(std::shared_ptr<Connection>)> callback = std::bind(&GameMain::on_client_connected, game_main.get(), std::placeholders::_1);
  asio_server->regist_on_client_connected(callback);

  // start asio server
  asio_server->start();

  // Test: wait for 60 seconds
  std::this_thread::sleep_for(std::chrono::seconds(60));

  return EXIT_SUCCESS;
}