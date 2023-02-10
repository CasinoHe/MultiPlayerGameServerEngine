// Create: 2023-02-09
// Author: CasinoHe
// Purpose: bottom tcp connection class using boost::asio library
#pragma once

#include "connection.h"
#include <boost/asio.hpp>
#include <memory>
#include <functional>
#include <array>
#include <list>

#define MAX_BUFFER_SIZE 1024

namespace multiplayer_server
{
  // forward declaration, abstract logger class
  class LoggerImp;

  class AsioTcpConnection : public Connection
  {
  public:
    AsioTcpConnection(const std::string &ip, uint16_t port, std::shared_ptr<boost::asio::io_context> io_context);
    virtual ~AsioTcpConnection();

    // get connection status
    virtual ConnectionStatus get_status() const override;

    // connect to remote host
    // return true if connect successfully
    virtual bool connect() override;
    // rsync connect to remote host
    virtual bool async_connect() override;

    // send data to remote host
    // return true if send successfully
    virtual bool send(const void* data, size_t size) override;
    // async send data to remote host
    // return true if send successfully
    virtual bool async_send(const void* data, size_t size) override;

    // receive data from remote host
    // return true if receive successfully
    virtual bool receive(void* data, size_t size) override;

    // close connection
    virtual void close() override;

    // start receive data from remote host
    virtual void start_receive() override;

    // get socket
    std::shared_ptr<boost::asio::ip::tcp::socket> get_socket() const { return socket_; }

  protected:
    // async connected callback, result is true if connect successfully
    virtual void on_connected(bool result) override;

    // receive callback
    virtual void on_received(const void* data, size_t size) override;

    // handle connect
    void handle_connect(const boost::system::error_code& error);

    // handle send
    void handle_send(const boost::system::error_code& error, size_t bytes_transferred);

    // handle receive
    void handle_receive(const boost::system::error_code& error, size_t bytes_transferred);

    // handle close
    void handle_close(const boost::system::error_code& error);

    // heartbeat, check connection status especially for udp
    virtual void heartbeat() override {}; // tcp do nothing
    virtual void set_keep_alive(bool enable) override;

  protected:
    // io service
    std::shared_ptr<boost::asio::io_context> io_context_ = nullptr;
    // socket
    std::shared_ptr<boost::asio::ip::tcp::socket> socket_ = nullptr;
    // remote endpoint
    boost::asio::ip::tcp::endpoint remote_endpoint_;

    // receive buffer
    std::array<char, MAX_BUFFER_SIZE> receive_buffer_;
    // send buffer
    std::list<boost::asio::const_buffer> send_buffer_;
    // is sending
    bool is_sending_ = false;

    // logger
    std::shared_ptr<LoggerImp> logger_ = nullptr;
  };
}