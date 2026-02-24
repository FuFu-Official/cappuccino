// Pipe operator for vector
// c++20
#include <concepts>
#include <functional>
#include <iostream>
#include <vector>

template <typename U, typename F>
  requires std::regular_invocable<F, U &>
std::vector<U> &operator|(std::vector<U> &v1, F f) {
  for (auto &i : v1) {
    f(i);
  }
  return v1;
}

int main() {
  std::vector v{1, 2, 3};
  std::function f{[](const int &i) { std::cout << i << ' '; }};
  auto f2 = [](int &i) { i *= i; };
  v | f2 | f;
}

// Output:
// 1 4 9
