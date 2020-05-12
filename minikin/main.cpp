#include <iostream>

extern "C" void dump_layout();

int main() {
  std::cout << "LATOUT FROM DYLIB";
  dump_layout();
  return 0;
}
