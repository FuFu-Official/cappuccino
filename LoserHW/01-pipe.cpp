#include <functional>
#include <iostream>
#include <vector>

int main() {
  std::vector v{1, 2, 3};
  std::function f{[](const int &i) { std::cout << i << ' '; }};
  auto f2 = [](int &i) { i *= i; };
  // v | f2 | f;
}
