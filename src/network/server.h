// Created: 2023-02-09
// Author: CasinoHe
// Purpose: a abstract class for network server
#pragma once
#include <string>

namespace multiplayer_server
{
  enum class ServerStatus
  {
    kNone,
    kRunning,
    kStopped,
    kError
  };

  class Server
  {
  public:
    Server(const std::string &ip_address, int port, bool has_tcp, bool has_udp)
        : ip_address_(ip_address), port_(port), has_tcp_(has_tcp), has_udp_(has_udp) {}

    virtual ~Server() = default;

    virtual bool start() = 0;
    virtual bool stop() = 0;

    // supporting tcp
    virtual bool has_tcp() const { return has_tcp_; };
    // supporting udp
    virtual bool has_udp() const { return has_udp_; };

    // ip address operations
    const std::string &ip_address() const { return ip_address_; }
    void set_ip_address(const std::string &ip_address) { ip_address_ = ip_address; }

    // port operations
    int port() const { return port_; }
    void set_port(int port) { port_ = port; }

    // status operations
    const ServerStatus status() const { return status_; }
    bool is_running() const { return status_ == ServerStatus::kRunning; }
    bool is_stopped() const { return status_ == ServerStatus::kStopped; }

  protected:
    void set_status(ServerStatus status) { status_ = status; }

  protected:
    std::string ip_address_ = "";                           // listen ip address
    int port_ = 0;                                          // listen port
    ServerStatus status_ = ServerStatus::kNone;             // server status
    bool has_tcp_ = false;                                  // listen on tcp
    bool has_udp_ = false;                                  // listen on udp
  };
}