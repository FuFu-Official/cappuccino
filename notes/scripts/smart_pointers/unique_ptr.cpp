#include <iostream>
#include <memory>
#include <string>
#include <vector>

struct MyMap {
  std::string key;
  std::string value;

  MyMap() = default;

  MyMap(const std::string &key, const std::string &value)
      : key(key), value(value) {}

  static std::unique_ptr<MyMap> MyMapFactory(const std::string &key,
                                             const std::string &value) {
    return std::make_unique<MyMap>(key, value);
  }

  void print() {
    std::cout << "Key: " << key << "-> Value: " << value << std::endl;
  }
};

int main() {
  // Crete a unique_ptr to MyMap
  std::unique_ptr<MyMap> myMap1 = std::make_unique<MyMap>("key1", "value1");

  // Obtain unique_ptr from function that returns by value
  auto myMap2 = myMap1->MyMapFactory("key2", "value2");

  // Create an array of unique_ptr to MyMap
  auto myMaps1 = std::make_unique<MyMap[]>(5);

  // Inistialize the array of unique_ptr
  for (int i = 0; i < 5; ++i) {
    myMaps1[i] = MyMap("key" + std::to_string(i), "value" + std::to_string(i));
  }

  // Move raw pointer from one unique_ptr to another.
  auto myMap1_moved = std::move(myMap1);

  // myMap1->print();
  // this will cause a runtime error because myMap1 is now empty

  std::vector<std::unique_ptr<MyMap>> myMaps2;

  // Crete new uniqque_ptr instances
  // and add them to vector using implict Move Semantics
  myMaps2.push_back(std::make_unique<MyMap>("key3", "value3"));
  myMaps2.push_back(std::make_unique<MyMap>("key4", "value4"));

  // Pass by reference to avoid copying unique_ptr
  for (const auto &map : myMaps2) {
    // map->print();
  }

  return 0;
}
