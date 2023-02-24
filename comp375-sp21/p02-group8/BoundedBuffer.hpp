#include <queue>
#include <mutex>
#include <condition_variable>

/**
 * Class representing a buffer with a fixed capacity.
 *
 * Note that in C++, the header (i.e. hpp) file contains a declaration of the
 * class while the implementation of the constructors, destructors, and methods,
 * and given in an implementation (i.e. cpp) file.
 */
class BoundedBuffer {
  // begin section containing publicly accessible parts of the class
  public:
	  // public constructor
	  BoundedBuffer(int max_size);
	  
	  // public member functions (a.k.a. methods)
	  int getItem();
	  void putItem(int new_item);

  // begin section containing private (i.e. hidden) parts of the class
  private:
	  // private member variables (i.e. fields)
	  long unsigned int capacity;
	  std::queue<int> buffer;
	  std::condition_variable dataAvailable;
	  std::condition_variable spaceAvailable;
	  std::mutex mutex;

	  // This class doesn't have any, but we could also have private
	  // constructors and/or member functions here.
};
