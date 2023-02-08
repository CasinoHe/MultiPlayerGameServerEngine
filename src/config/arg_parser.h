// Created: 2023.01.08
// Author: CasinoHe
// Desc: Command line argument parser
#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <boost/program_options.hpp>

namespace multiplayer_server
{
  // Command line argument parser
  // use boost::program_options to parse args
  class ArgParser
  {
  public:
    ArgParser(int argc, const char **argv);
    ~ArgParser();

    // Parse command line arguments
    bool parse();

    // Get config file path
    inline const std::string &get_config_file_path() const;

  private:
    int argc_ = 0;
    const char **argv_ = nullptr;
    std::string config_file_path_;
  };
}