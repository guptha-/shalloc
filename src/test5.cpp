// Used to test whether small objects go into bigger blocks once the smaller 
// blocks are exhausted
#include <iostream>
#include "../lib/shalloclib.hpp"

using namespace shalloclib;
class Obj1 : public SharedClass {
  public:
  char a[BLOCK_SIZE_ONE - 24];
};

class Obj2 : public SharedClass {
  public:
  char a[BLOCK_SIZE_TWO - 24];
};

class Obj3 : public SharedClass {
  public:
  char a[BLOCK_SIZE_THREE - 24];
};

int main() {
  cout<<"Starting test case 3"<<endl;

  const int totalBlocks = NUMBER_BLOCKS_ONE + NUMBER_BLOCKS_TWO + 
    NUMBER_BLOCKS_THREE;
  Obj1 *obj1ptr[totalBlocks + 1];
  cout<<"Start obj1"<<endl;
  for (int i = 0; i < totalBlocks; i++) {
    obj1ptr[i] = new Obj1();
    if (obj1ptr[i] == NULL) {
      cout<<"ERROR Could not allocate req blocks pool 1"<<endl;
    }
  }
  obj1ptr[totalBlocks] = new Obj1();
  if (obj1ptr[totalBlocks] != NULL) {
    cout<<"ERROR Could allocate more than req number of blocks pool 1"<<endl;
  }

  Obj3 *obj3ptr;
  obj3ptr = new Obj3();
  if (obj3ptr != NULL) {
    cout<<"ERROR Could allocate more than req number of blocks pool 3"<<endl;
  }

  Obj2 *obj2ptr;
  obj2ptr = new Obj2();
  if (obj2ptr != NULL) {
    cout<<"ERROR Could allocate more than req number of blocks pool 2"<<endl;
  }

  return 0;
}
