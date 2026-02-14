# Smart Pointers

Smart pointers are defined in the `std` namespace in the `<memory>` header file. They are crucial to the `RAII` or Resource Acquisition Is Initialization programming idiom. The main goal of this idiom is to ensure that resource acquisition occurs at the same time that the object is initialized, so that all resources for the object are created and made ready in one line of code.

> [!IMPORTANT]
> Always create smart pointers on a separate line of code, never in a parameter list, so that a subtle resource leak won't occur due to certain parameter list allocation rules.

Essential steps to use smart pointers:

- Declare the smart pointer as an automatic (local) variable. (Do not use the `new` or `malloc` expression on the smart pointer itself.)
- In the type parameter, specify the pointed-to type of the encapsulated pointer.
- Pass a raw pointer to a new-ed object in the smart pointer constructor. (Some utility functions or smart pointer constructors do this for you)
- Use the overloaded `->` and `*` operators to access the object. (the dots `.` operator is for smart pointers class own member functions)
- Let the smart pointer delete the object.

## Unique Pointer

A `unique_ptr` does not share its pointer. It cannot be copied to another `unique_ptr`, passed by value to a function, or used in any C++ Standard Library algorithm that requires copies to be made. A `unique_ptr` can only be moved. This means that the ownership of the memory resource is transferred to another `unique_ptr` and the original `unique_ptr` no longer owns it.

```cpp
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
```

## Shared Pointer

The `shared_ptr` type is a smart pointer in the C++ standard library that is designed for scenarios in which more than one owner needs to manage the lifetime of an object. After you initialize a `shared_ptr` you can copy it, pass it by value in function arguments, and assign it to other `shared_ptr` instances. All the instances point to the same object, and share access to one "control block" that increments and decrements the reference count whenever a new `shared_ptr` is added, goes out of scope, or is reset. When the reference count reaches zero, the control block deletes the memory resource and itself.

```cpp
#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

struct MediaAsset {
  virtual ~MediaAsset() = default; // make it polymorphic
};

struct Song : public MediaAsset {
  std::string artist;
  std::string title;
  Song(const std::string &artist_, const std::string &title_)
      : artist{artist_}, title{title_} {}
};

struct Photo : public MediaAsset {
  std::string date;
  std::string location;
  std::string subject;
  Photo(const std::string &date_, const std::string &location_,
        const std::string &subject_)
      : date{date_}, location{location_}, subject{subject_} {}
};

void use_shared_ptr_by_value(std::shared_ptr<int> sp);

void use_shared_ptr_by_reference(std::shared_ptr<int> &sp);
void use_shared_ptr_by_const_reference(const std::shared_ptr<int> &sp);

void use_raw_pointer(int *p);
void use_reference(int &r);

void test() {
  auto sp= std::make_shared<int>(5);

  // Pass the shared_ptr by value.
  // This invokes the copy constructor, increments the reference count, and
  // makes the callee an owner.
  use_shared_ptr_by_value(sp);

  // Pass the shared_ptr by reference or const reference.
  // In this case, the reference count isn't incremented.
  use_shared_ptr_by_reference(sp);
  use_shared_ptr_by_const_reference(sp);

  // Pass the underlying pointer or a reference to the underlying object.
  use_raw_pointer(sp.get());
  use_reference(*sp);

  // Pass the shared_ptr by value.
  // This invokes the move constructor, which doesn't increment the reference
  // count but in fact transfers ownership to the callee.
  use_shared_ptr_by_value(std::move(sp));
}

int main() {
  // The examples go here, in order:
  // Use make_shared function when possible.
  auto sp1 =
      std::make_shared<Song>("The Beatles", "Im Happy Just to Dance With You");

  // Ok, but slightly less efficient.
  // Note: Using new expression as constructor argument
  // creates no named variable for other code to access.
  std::shared_ptr<Song> sp2(new Song("ady Gaga", "Just Dance"));

  // When initialization must be separate from declaration, e.g. class members,
  // initialize with nullptr to make your programming intent explicit.
  std::shared_ptr<Song> sp5(nullptr);
  // Equivalent to: shared_ptr<Song> sp5;
  sp5 = std::make_shared<Song>("Elton John", "I'm Still Standing");

  // Initialize with copy constructor. Increments ref count.
  auto sp3(sp2);

  // Initialize via assignment. Increments ref count.
  auto sp4 = sp2;

  // Initialize with nullptr. sp7 is empty.
  std::shared_ptr<Song> sp7(nullptr);

  // Initialize with another shared_ptr. sp1 and sp2
  // swap pointers as well as ref counts.
  sp1.swap(sp2);

  std::vector<std::shared_ptr<Song>> v{
      std::make_shared<Song>("Bob Dylan", "The Times They Are A Changing"),
      std::make_shared<Song>("Aretha Franklin", "Bridge Over Troubled Water"),
      std::make_shared<Song>("Thalía", "Entre El Mar y Una Estrella")};

  std::vector<std::shared_ptr<Song>> v2;
  std::remove_copy_if(v.begin(), v.end(), std::back_inserter(v2),
                      [](std::shared_ptr<Song> s) {
                        return s->artist.compare("Bob Dylan") == 0;
                      });

  for (const auto &s : v2) {
    std::cout << s->artist << ":" << s->title << std::endl;
  }

  std::vector<std::shared_ptr<MediaAsset>> assets{
      std::make_shared<Song>("Himesh Reshammiya", "Tera Surroor"),
      std::make_shared<Song>("Penaz Masani", "Tu Dil De De"),
      std::make_shared<Photo>("2011-04-06", "Redmond, WA",
                              "Soccer field at Microsoft.")};

  std::vector<std::shared_ptr<MediaAsset>> photos;

  std::copy_if(assets.begin(), assets.end(), std::back_inserter(photos),
               [](std::shared_ptr<MediaAsset> p) -> bool {
                 // Use dynamic_pointer_cast to test whether element is a
                 // shared_ptr<Photo>.
                 std::shared_ptr<Photo> temp =
                     std::dynamic_pointer_cast<Photo>(p);
                 return temp.get() != nullptr;
               });

  for (const auto &p : photos) {
    // We know that the photos vector contains only
    // shared_ptr<Photo> objects, so use static_cast.
    std::cout << "Photo location: "
              << (std::static_pointer_cast<Photo>(p))->location << std::endl;
  }

  // Initialize two separate raw pointers.
  // Note that they contain the same values.
  auto song1 = new Song("Village People", "YMCA");
  auto song2 = new Song("Village People", "YMCA");

  // Create two unrelated shared_ptrs.
  std::shared_ptr<Song> p1(song1);
  std::shared_ptr<Song> p2(song2);

  // Unrelated shared_ptrs are never equal.
  std::cout << "p1 < p2 = " << std::boolalpha << (p1 < p2) << std::endl;
  std::cout << "p1 == p2 = " << std::boolalpha << (p1 == p2) << std::endl;

  // Related shared_ptr instances are always equal.
  std::shared_ptr<Song> p3(p2);
  std::cout << "p3 == p2 = " << std::boolalpha << (p3 == p2) << std::endl;
}
```

## Weak Pointer

Sometimes an object must store a way to access the underlying object of a `cppshared_ptr` without causing the reference count to be incremented. Typically, this situation occurs when you have cyclic references between `shared_ptr` instances.

```cpp
// Example code that demonstrates a cyclic reference between shared_ptr instances, which can lead to memory leaks.
struct B;

struct A {
    std::shared_ptr<B> pointerToB;
    ~A() { std::cout << "A destroyed\n"; }
};

struct B {
    std::shared_ptr<A> pointerToA;
    ~B() { std::cout << "B destroyed\n"; }
};

void leakMemory() {
    auto a = std::make_shared<A>();
    auto b = std::make_shared<B>();

    a->pointerToB = b;
    b->pointerToA = a; // cyclic reference, memory leak occurs
}
```

```cpp
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
  for (int i = 0; i < v.size(); ++i) {
    std::for_each(v.begin(), v.end(), [&v, i](std::shared_ptr<Controller> p) {
      if (p->Num != i) {
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
```

# References

- [Smart pointers (Modern C++)](https://learn.microsoft.com/en-us/cpp/cpp/smart-pointers-modern-cpp?view=msvc-170)
