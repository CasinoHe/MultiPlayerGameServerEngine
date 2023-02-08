#include <iostream>
#include "config/arg_parser.h"

int main(int argc, const char **argv)
{
  multiplayer_server::ArgParser arg_parser(argc, argv);

  // if parse failed, exit
  if (!arg_parser.parse())
  {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}