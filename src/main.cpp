#include "monitor.hpp"
#include "utils.hpp"

using mem::Monitor;

int main() {
  Monitor<std::vector<std::string>> mon{"x", "y", "z"};
  return EXIT_SUCCESS;
}