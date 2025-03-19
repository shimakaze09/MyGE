//
// Created by Admin on 19/03/2025.
//

#include <MyGE/_deps/crossguid/guid.hpp>

#include <iostream>

int main() {
  std::cout << xg::newGuid().str() << std::endl;
  return 0;
}
