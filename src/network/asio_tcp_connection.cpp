// Created: 2023-02-09
// Author: CasinoHe
// Purpose: bottom tcp connection class using boost::asio library
#include "asio_tcp_connection.h"
#include "log/logger.h"
#include <format>

namespace multiplayer_server
{

  AsioTcpConnection::AsioTcpConnection(const std::string &ip, uint16_t port, std::shared_ptr<boost::asio::io_context> io_context)
      : Connection(ip, port)
  {
    // if io_context is nullptr, create a new one
    if (!io_context)
    {
      io_context_ = std::make_shared<boost::asio::io_context>();
      socket_ = std::make_shared<boost::asio::ip::tcp::socket>(*io_context_);
    }
    else
    {
      io_context_ = io_context;
      socket_ = std::make_shared<boost::asio::ip::tcp::socket>(*io_context_);
    }

    // set options for multiplayer game connection
    socket_->set_option(boost::asio::ip::tcp::no_delay(true));
    socket_->set_option(boost::asio::socket_base::keep_alive(true));
    socket_->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));

    logger_ = g_logger_manager.create_logger("AsioTcpConnection", LoggerLevel::Debug, "log/AsioTcpConnection.log");
  }

  AsioTcpConnection::~AsioTcpConnection()
  {
    close();
  }

  // get connection status
  ConnectionStatus AsioTcpConnection::get_status() const
  {
    return status_;
  }

  // connect to remote host
  // return true if connect successfully
  bool AsioTcpConnection::connect()
  {
    // check if already connected
    if (status_ == ConnectionStatus::kConnected || status_ == ConnectionStatus::kConnecting)
    {
      return true;
    }

    boost::system::error_code error;
    boost::asio::ip::tcp::resolver resolver(*io_context_);
    boost::asio::ip::tcp::resolver::query query(ip_, std::to_string(port_));
    boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query, error);

    if (error)
    {
      return false;
    }

    logger_->debug(std::format("connect to {}:{}", ip_, port_));
    socket_->connect(*endpoint_iterator, error);

    if (error)
    {
      return false;
    }
    set_status(ConnectionStatus::kConnected);
    logger_->debug(std::format("connect to {}:{} successfully", ip_, port_));
    return true;
  }

  // rsync connect to remote host
  bool AsioTcpConnection::async_connect()
  {
    // check if already connected
    if (status_ == ConnectionStatus::kConnected || status_ == ConnectionStatus::kConnecting)
    {
      return true;
    }

    boost::asio::ip::tcp::resolver resolver(*io_context_);
    boost::asio::ip::tcp::resolver::query query(ip_, std::to_string(port_));
    boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

    socket_->async_connect(*endpoint_iterator, std::bind(&AsioTcpConnection::handle_connect, this, std::placeholders::_1));

    set_status(ConnectionStatus::kConnecting);
    logger_->debug(std::format("async connect to {}:{}", ip_, port_));
    return true;
  }

  // rsync connect handler
  void AsioTcpConnection::handle_connect(const boost::system::error_code &error)
  {
    if (error)
    {
      logger_->debug(std::format("async connect to {}:{} failed", ip_, port_));
      set_status(ConnectionStatus::kDisconnected);
      on_connected(false);
      return;
    }
    logger_->debug(std::format("async connect to {}:{} successfully", ip_, port_));
    set_status(ConnectionStatus::kConnected);
    on_connected(true);
  }

  // on connected
  void AsioTcpConnection::on_connected(bool success)
  {
    if (connected_callback_)
    {
      try 
      {
        connected_callback_(success);
      }
      catch(const std::exception& e)
      {
        logger_->error(std::format("on_connected callback error {}", e.what()));
      }
    }

    if (success)
    {
      start_receive();
    }
  }

  // send data to remote host
  // return true if send successfully
  bool AsioTcpConnection::send(const void *data, size_t size)
  {
    boost::system::error_code error;
    socket_->write_some(boost::asio::buffer(data, size), error);
    if (error)
    {
      logger_->debug(std::format("send data to {}:{} failed, size {}", ip_, port_, size));
      return false;
    }
    logger_->debug(std::format("send data to {}:{} successfully, size {}", ip_, port_, size));
    return true;
  }

  // async send data to remote host
  // return true if send successfully
  bool AsioTcpConnection::async_send(const void *data, size_t size)
  {
    // first, add to send buffer list
    if (send_buffer_.size() > 0)
    {
      send_buffer_.emplace_back(data, size);
      return false;
    }

    // second, check if is sending
    if (is_sending_)
    {
      logger_->error(std::format("async send data add to queue, but queue size is {}", send_buffer_.size()));

      // add to send queue and wait for send
      send_buffer_.emplace_back(data, size);
      return false;
    }

    is_sending_ = true;
    socket_->async_write_some(send_buffer_.front(),
                             std::bind(&AsioTcpConnection::handle_send, this,
                                         std::placeholders::_1,
                                         std::placeholders::_2));
    logger_->debug(std::format("async send data to {}:{} size {}", ip_, port_, size));
     return true;
  }

  // async send handler
  void AsioTcpConnection::handle_send(const boost::system::error_code &error, size_t bytes_transferred)
  {
    // first, set sending flag to false
    is_sending_ = false;

    // second, check if has data in send buffer
    // if no error, pop sent data at the front of send_buffer_
    if (send_buffer_.size() > 0)
    {
      if (!error)
      {
        send_buffer_.pop_front();
      }
    }

    // then, send next data in send buffer, maybe resend failed data
    if (send_buffer_.size() > 0)
    {
      is_sending_ = true;
      socket_->async_write_some(send_buffer_.front(),
                                std::bind(&AsioTcpConnection::handle_send, this,
                                          std::placeholders::_1,
                                          std::placeholders::_2));
      logger_->debug(std::format("async send data to {}:{} size {}", ip_, port_, send_buffer_.front().size()));
    }

    if (error)
    {
      logger_->debug(std::format("async send data to {}:{} failed, size {} error code {} try close", ip_, port_, error.what(), bytes_transferred));
    }
    else
    {
      logger_->debug(std::format("async send data to {}:{} successfully, size {}", ip_, port_, bytes_transferred));
    }
  }

  // receive data from remote host
  // return true if receive successfully
  bool AsioTcpConnection::receive(void *data, size_t size)
  {
    boost::system::error_code error;
    socket_->read_some(boost::asio::buffer(data, size), error);
    if (error)
    {
      logger_->debug(std::format("receive data from {}:{} failed, size {}", ip_, port_, size));
      return false;
    }
    logger_->debug(std::format("receive data from {}:{} successfully, size {}", ip_, port_, size));
    on_received(data, size);
    return true;
  }

  // start receive from remote host
  void AsioTcpConnection::start_receive()
  {
    socket_->async_read_some(boost::asio::buffer(receive_buffer_, MAX_BUFFER_SIZE),
                             std::bind(&AsioTcpConnection::handle_receive, this,
                                         std::placeholders::_1,
                                         std::placeholders::_2));
  }

  // handle async receive data
  void AsioTcpConnection::handle_receive(const boost::system::error_code &error, size_t bytes_transferred)
  {
    if (error)
    {
      logger_->debug(std::format("receive data from {}:{} failed, error code {}", ip_, port_, error.what()));
      close();
      return;
    }

    start_receive();

    on_received(receive_buffer_.data(), bytes_transferred);
  }

  // close connection
  void AsioTcpConnection::close()
  {
    if (status_ == ConnectionStatus::kClosed)
    {
      return;
    }
    boost::system::error_code error;
    socket_->shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);
    socket_->close(error);
    set_status(ConnectionStatus::kClosed);
  }

  // keep alive
  void AsioTcpConnection::set_keep_alive(bool enable)
  {
    socket_->set_option(boost::asio::socket_base::keep_alive(enable));
    if (enable)
    {
#ifdef _UNIX
      socket_->set_option(boost::asio::detail::socket_option::integer<SOL_TCP, TCP_KEEPIDLE>(keep_idle_interval_));
      socket_->set_option(boost::asio::detail::socket_option::integer<SOL_TCP, TCP_KEEPINTVL>(keep_interval_));
      socket_->set_option(boost::asio::detail::socket_option::integer<SOL_TCP, TCP_KEEPCNT>(keep_cnt_));
#endif
    }
  }

  // on received data
  void AsioTcpConnection::on_received(const void *data, size_t size)
  {
    if (received_callback_)
    {
      try 
      {
        received_callback_(data, size);
      }
      catch(const std::exception& e)
      {
        logger_->error(std::format("on_received callback error {}", e.what()));
      }
    }
  }
}