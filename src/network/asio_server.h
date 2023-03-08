// Created: 2023-02-09
// Author: CasinoHe
// Purpose: implement a network server based on boost::asio
#pragma once
#include "server.h"
#include "log/logger.h"
#include <boost/asio.hpp>
#include <memory>
#include <string>
#include <functional>

namespace multiplayer_server
{
  // connection forward declaration
  class Connection;
  class AsioTcpConnection;

  class AsioServer : public Server
  {
  public:
    AsioServer(const std::string &ip_address, int port, bool has_tcp = true, bool has_udp = false);
    virtual ~AsioServer();

    virtual bool set_io_context_thread_count(int count) { io_context_thread_count_ = count; return true; }
    virtual bool start() override;
    virtual bool stop() override;

    // start tcp or udp accept
    void start_tcp_accept();
    void start_udp_accept();

    // handle accept
    void handle_tcp_accept(const boost::system::error_code &error, std::shared_ptr<AsioTcpConnection> connection);
    void handle_udp_accept(const boost::system::error_code &error);

    // game module callback
    void register_on_tcp_connection_accepted(std::function<bool(std::shared_ptr<Connection>)> &callback) { on_connection_accepted_callback_ = callback; }
    // game module callback when server closed
    void register_on_server_closed(std::function<void()> &callback) { on_server_closed_callback_ = callback; }

  protected:
    void start_io_context_thread_pool();

  protected:
    std::shared_ptr<boost::asio::io_context> io_context_;
    std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
    std::shared_ptr<LoggerImp> logger_;

    // io context thread pool
    std::vector<std::thread> io_context_threads_pool_;
    int io_context_thread_count_ = 0;
    
    // callback game module when a tcp connection is accepted
    std::function<bool(std::shared_ptr<Connection>)> on_connection_accepted_callback_;
    // callback game module when server closed
    std::function<void()> on_server_closed_callback_;
  };
}