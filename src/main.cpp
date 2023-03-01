#include "config/arg_parser.h"
#include "config/json_config_parser.h"
#include "log/logger.h"
#include "network/asio_server.h"
#include "game/game_main.h"
#include <iostream>
#include <filesystem>
#include <chrono>
#include <thread>
#include <functional>

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

  // get config value, especially for ip and port
  auto json_parser = std::make_unique<JsonConfigParser>(config_file_path);
  if (!json_parser->parse())
  {
    return EXIT_FAILURE;
  }

  std::string ip = json_parser->get<std::string>("ip", "127.0.0.1");
  int port = json_parser->get("port", 8080);

  // create game_main object, init login service
  auto game_main = std::make_unique<GameMain>(ip, port);
  game_main->init_all_game_services();

  // create asio server
  auto asio_server = std::make_unique<AsioServer>(ip, port, true, false);
  asio_server->set_io_context_thread_count(10);

  // register connected callback
  std::function<bool(std::shared_ptr<AsioTcpConnection>)> callback = std::bind(&GameMain::on_client_connected<AsioTcpConnection>, game_main.get(), std::placeholders::_1);
  asio_server->register_on_tcp_connection_accepted(callback);

  // start asio server
  asio_server->start();

  // Test: wait for 60 seconds
  std::this_thread::sleep_for(std::chrono::seconds(60));

  return EXIT_SUCCESS;
}