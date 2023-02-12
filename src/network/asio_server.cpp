#include "asio_server.h"
#include "asio_tcp_connection.h"

namespace multiplayer_server
{
#define MIN_IO_CONTEXT_THREAD_COUNT 2

    AsioServer::AsioServer(const std::string &ip, int port, bool has_tcp, bool has_udp)
        : Server(ip, port, has_tcp, has_udp)
    {
      io_context_ = std::make_shared<boost::asio::io_context>();
      logger_ = g_logger_manager.create_logger("AsioServer", LoggerLevel::Debug, "log/AsioServer.log");
    }
  
    AsioServer::~AsioServer()
    {
      stop();
    }
  
    bool AsioServer::start()
    {
      // check if already started
      if (status_ == ServerStatus::kRunning)
      {
        return true;
      }
  
      // start tcp accept
      if (has_tcp_)
      {
        start_tcp_accept();
      }
      // start udp accept
      if (has_udp_)
      {
        start_udp_accept();
      }
  
      // runn io context in multiple threads
      start_io_context_thread_pool();
  
      return true;
    }
  
    bool AsioServer::stop()
    {
      // check if already stopped
      if (status_ == ServerStatus::kStopped)
      {
        return true;
      }

      // stop all io context threads
      io_context_->stop();

      // wait for all threads to exit
      for (auto &thread : io_context_threads_pool_)
      {
        if (thread.joinable())
        {
          thread.join();
        }
      }

      // finished all threads and io then call game module callback
      if (on_server_closed_callback_)
      {
        on_server_closed_callback_();
      }

      return true;
    }
  
    void AsioServer::start_tcp_accept()
    {
      // first, resolve the ip address and port
      boost::asio::ip::tcp::resolver resolver(*io_context_);
      boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(ip_address_, std::to_string(port_)).begin();

      // create acceptor
      acceptor_ = std::make_shared<boost::asio::ip::tcp::acceptor>(*io_context_, endpoint);
      // set reuse address
      acceptor_->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
      // set no delay
      acceptor_->set_option(boost::asio::ip::tcp::no_delay(true));
      // set keep alive
      acceptor_->set_option(boost::asio::socket_base::keep_alive(true));

      // create a new connection
      auto connection = std::make_shared<AsioTcpConnection>(ip_address_, port_, io_context_);
  
      // start accept
      acceptor_->async_accept(*connection->get_socket(), std::bind(&AsioServer::handle_tcp_accept, this, std::placeholders::_1, connection));
      // set status
      status_ = ServerStatus::kRunning;
      // log start time and port
      logger_->info("start tcp accept on {}:{}", ip_address_, port_);
    }
  
    void AsioServer::start_udp_accept()
    {
      // create a new connection
      // auto connection = std::make_shared<AsioUdpConnection>(ip_address_, port_, io_context_);
  
      // start accept
      // acceptor_->async_accept(*connection->get_socket(), std::bind(&AsioServer::handle_udp_accept, this, std::placeholders::_1));
    }

    // maybe one player connected to server, give the client to game logic module
    void AsioServer::handle_tcp_accept(const boost::system::error_code &error, std::shared_ptr<AsioTcpConnection> connection)
    {
      if (error)
      {
        return;
      }
  
      // start next accept
      start_tcp_accept();

      // give connection to game logic module
      if (on_tcp_connection_accepted_callback_)
      {
        if (on_tcp_connection_accepted_callback_(connection))
        {
          // start read
          connection->start_receive();
        }
      }
    }
  
    void AsioServer::handle_udp_accept(const boost::system::error_code &error)
    {
      if (error)
      {
        return;
      }
  
      // start next accept
      start_udp_accept();
    }

    // start io context in multiple threads
    void AsioServer::start_io_context_thread_pool()
    {
      // get the number of hardware threads
      auto thread_count = std::thread::hardware_concurrency();
      if (thread_count < MIN_IO_CONTEXT_THREAD_COUNT)
      {
        thread_count = MIN_IO_CONTEXT_THREAD_COUNT;
      }

      // compare thread count with member variable io_context_thread_count_
      if (io_context_thread_count_ <= 0)
      {
        io_context_thread_count_ = thread_count;
      }
      else
      {
        thread_count = io_context_thread_count_;
      }
  
      // create thread pool
      for (int i = 0; i < thread_count; i++)
      {
        io_context_threads_pool_.emplace_back([this]() {
          io_context_->run();
        });
      }
    }
}