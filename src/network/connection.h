// Created: 2023-02-09
// Author: CasinoHe
// Purpose: a abstract class of network connection
#pragma once

#include <string>
#include <functional>

namespace multiplayer_server
{
  enum class ConnectionStatus
  {
    kNone,
    kConnecting,
    kConnected,
    kDisconnected,
    kClosed
  };

  // abstract class of network connection
  // define common interface
  // Connection is a nocopyable class
  class Connection
  {
  public:
    Connection(const std::string &ip, uint16_t port)
    {
      ip_ = ip;
      port_ = port;
    }
    virtual ~Connection() = default;

    // nocopyable
    Connection(const Connection &) = delete;
    Connection(const Connection &&) = delete;
    Connection &operator=(const Connection &) = delete;
    Connection &operator=(const Connection &&) = delete;

    // get connection status
    virtual ConnectionStatus get_status() const = 0;

    // connect to remote host
    // return true if connect successfully
    virtual bool connect() = 0;
    // rsync connect
    virtual bool async_connect() = 0;
    // async connected callback, result is true if connect successfully
    virtual void on_connected(bool result) = 0;
    // set rsync connected callback, argument is a bool, true means connected succeed
    virtual void set_connected_callback(std::function<void(bool)> callback) { connected_callback_ = callback; }

    // send data to remote host
    // return true if send successfully
    virtual bool send(const void *data, size_t size) = 0;
    // async send data to remote host
    // return true if send successfully
    virtual bool async_send(const void *data, size_t size) = 0;

    // receive data from remote host
    // return true if receive successfully
    virtual bool receive(void *data, size_t size) = 0;
    // receive callback
    virtual void on_received(const void *data, size_t size) = 0;
    // set receive data callback
    virtual void set_receive_callback(std::function<void(const void *, size_t)> callback) { received_callback_ = callback; };

    // close connection
    virtual void close() = 0;

    // start read data from remote host
    virtual void start_receive() = 0;

    // get connection status
    virtual void get_status(ConnectionStatus status) { status_ = status; }

    virtual void set_keep_alive(bool enable) = 0;

    // heartbeat, check connection status especially for udp
    virtual void heartbeat() = 0;

    virtual void set_keep_interval(int interval) { keep_interval_ = interval; }
    virtual void set_keep_cnt(int cnt) { keep_cnt_ = cnt; }
    virtual void set_keep_idle_interval(int interval) { keep_idle_interval_ = interval; }

  protected:
    // update connection status
    virtual void set_status(ConnectionStatus status) { status_ = status; }

  protected:
    // connection status
    ConnectionStatus status_{ConnectionStatus::kNone};
    std::string ip_; // remote host ip
    uint16_t port_;  // remote host port

    // connected callback
    std::function<void(bool)> connected_callback_ = nullptr;
    // disconnected callback
    std::function<void()> disconnected_callback_ = nullptr;
    // receive data callback
    std::function<void(const void *, size_t)> received_callback_ = nullptr;

    // heart beat check variables
    int keep_idle_interval_ = 60;
    int keep_interval_ = 60;
    int keep_cnt_ = 3;
  };
}