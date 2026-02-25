#include <format>
#include <iostream>

constexpr auto operator""_f(const char *fmt, size_t) {
  return [=]<typename... T>(T &&...Args) {
    return std::vformat(fmt, std::make_format_args(Args...));
  };
}

int main() {
  std::cout << "hello :{} *\n"_f(5);
  std::cout << "hello :{0} {0} *\n"_f(5);
  std::cout << "hello :{:b} *\n"_f(0b01010101);
  std::cout << "{:*<10}"_f("loser");
  std::cout << '\n';
  int n{};
  std::cin >> n;
  std::cout << "π：{:.{}f}\n"_f(std::numbers::pi_v<double>, n);
}

// Output:
// hello :5 *
// hello :5 5 *
// hello :1010101 *
// loser*****
// 2 (Input 2)
// π：3.14
