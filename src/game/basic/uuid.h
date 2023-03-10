// Created: 2023.02.18
// Author: CasinoHe
// Purpose: create a unique id
#pragma once
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <memory>
#include <string>

namespace multiplayer_server
{
  // a class hold a unique, global uuid
  // use boost::uuid or other library
  class UUID
  {
  public:
    UUID()
    {
      boost::uuids::uuid uuid = generator_();
      // convert uuid to string
      id_ = boost::uuids::to_string(uuid);
    }

    ~UUID() {}

    // generate next id and return current id
    std::string &get()
    {
      auto &old = id_;
      id_ = boost::uuids::to_string(generator_());
      return old;
    }

  private:
    std::string id_;
    boost::uuids::random_generator generator_;
  };
}