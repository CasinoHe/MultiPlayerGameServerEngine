#include "arg_parser.h"

namespace multiplayer_server
{
  ArgParser::ArgParser(int argc, const char **argv) : argc_(argc), argv_(argv), config_file_path_("")
  {
  }

  ArgParser::~ArgParser()
  {
  }

  const std::string &ArgParser::get_config_file_path() const
  {
    return config_file_path_;
  }

  bool ArgParser::parse()
  {
    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()
      ("help", "produce help message")
      ("config", po::value<std::string>(), "set config file path")
      ;

    po::variables_map vm;
    try
    {
      po::store(po::parse_command_line(argc_, argv_, desc), vm);
      po::notify(vm);
    }
    catch (const std::exception &e)
    {
      std::cout << e.what() << std::endl;
      std::cout << desc << std::endl;
      return false;
    }

    if (vm.count("help"))
    {
      std::cout << desc << std::endl;
      return false;
    }

    if (vm.count("config"))
    {
      config_file_path_ = vm["config"].as<std::string>();
    }
    else
    {
      std::cout << "config file path not set" << std::endl;
      std::cout << desc << std::endl;
      return false;
    }

    return true;
  }
}