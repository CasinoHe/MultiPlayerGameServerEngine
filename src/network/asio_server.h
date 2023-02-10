// Created: 2023-02-09
// Author: CasinoHe
// Purpose: implement a network server based on boost::asio
#pragma once
#include "server.h"
#include <boost/asio.hpp>
#include <memory>

namespace multiplayer_server
{
  class AsioServer : public Server
  {
  public:
    AsioServer() = default;
    virtual ~AsioServer() = default;

    virtual bool start() override;
    virtual bool stop() override;

    // start tcp or udp accept
    void start_tcp_accept();
    void start_udp_accept();

    // handle accept
    void handle_tcp_accept(const boost::system::error_code &error);
    void handle_udp_accept(const boost::system::error_code &error);

  protected:
    std::shared_ptr<boost::asio::io_context> io_context_;
    std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
  };
}