#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Controller {
public:
  int Num;
  std::string Status;
  std::vector<std::weak_ptr<Controller>> others;
  explicit Controller(int i) : Num(i), Status("On") {
    std::cout << "Creating Controller" << Num << std::endl;
  }

  ~Controller() { std::cout << "Destroying Controller" << Num << std::endl; }

  // Demonstrates how to test whether the
  // pointed-to memory still exists or not.
  void CheckStatuses() const {
    std::for_each(others.begin(), others.end(),
                  [](std::weak_ptr<Controller> wp) {
                    auto p = wp.lock();
                    if (p) {
                      std::cout << "Status of " << p->Num << " = " << p->Status
                                << std::endl;
                    } else {
                      std::cout << "Null object" << std::endl;
                    }
                  });
  }
};

void RunTest() {
  std::vector<std::shared_ptr<Controller>> v{
      std::make_shared<Controller>(0), std::make_shared<Controller>(1),
      std::make_shared<Controller>(2), std::make_shared<Controller>(3),
      std::make_shared<Controller>(4),
  };

  // Each controller depends on all others not being deleted.
  // Give each controller a pointer to all the others.
  for (unsigned long i = 0; i < v.size(); ++i) {
    std::for_each(v.begin(), v.end(), [&v, i](std::shared_ptr<Controller> p) {
      if (p->Num != static_cast<int>(i)) {
        v[i]->others.push_back(std::weak_ptr<Controller>(p));
        std::cout << "push_back to v[" << i << "]: " << p->Num << std::endl;
      }
    });
  }

  std::for_each(v.begin(), v.end(), [](std::shared_ptr<Controller> &p) {
    std::cout << "use_count = " << p.use_count() << std::endl;
    p->CheckStatuses();
  });
}

int main() {
  RunTest();
  std::cout << "Press any key" << std::endl;
  char ch;
  std::cin.getline(&ch, 1);
}
