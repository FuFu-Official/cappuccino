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
  auto sp = std::make_shared<int>(5);

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
