// Used to test the creation and deletion of differently sized shared objects
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

class Obj4 : public SharedClass {
  public:
  char a[BLOCK_SIZE_THREE - 24 + 1];
};

int main() {
  cout<<"Starting test case 3"<<endl;

  cout<<"Start obj4"<<endl;
  Obj4 *obj4 = new Obj4();
  if (obj4 != NULL) {
    cout<<"ERROR Able to allocate bigger chunks than allowed pool 4"<<endl;
  }

  Obj3 *obj3ptr[NUMBER_BLOCKS_THREE + 1];
  cout<<"Start obj3"<<endl;
  for (int i = 0; i < NUMBER_BLOCKS_THREE; i++) {
    obj3ptr[i] = new Obj3();
    if (obj3ptr[i] == NULL) {
      cout<<"ERROR ERROR Could not allocate req blocks pool 3"<<endl;
    }
  }
  obj3ptr[NUMBER_BLOCKS_THREE] = new Obj3();
  if (obj3ptr[NUMBER_BLOCKS_THREE] != NULL) {
    cout<<"ERROR Could allocate more than req number of blocks pool 3"<<endl;
  }

  Obj2 *obj2ptr[NUMBER_BLOCKS_TWO + 1];
  cout<<"Start obj2"<<endl;
  for (int i = 0; i < NUMBER_BLOCKS_TWO; i++) {
    obj2ptr[i] = new Obj2();
    if (obj2ptr[i] == NULL) {
      cout<<"ERROR ERROR Could not allocate req blocks pool 2"<<endl;
    }
  }
  obj2ptr[NUMBER_BLOCKS_TWO] = new Obj2();
  if (obj2ptr[NUMBER_BLOCKS_TWO] != NULL) {
    cout<<"ERROR Could allocate more than req number of blocks pool 2"<<endl;
  }

  Obj1 *obj1ptr[NUMBER_BLOCKS_ONE + 1];
  cout<<"Start obj1"<<endl;
  for (int i = 0; i < NUMBER_BLOCKS_ONE; i++) {
    obj1ptr[i] = new Obj1();
    if (obj1ptr[i] == NULL) {
      cout<<"ERROR ERROR Could not allocate req blocks pool 1"<<endl;
    }
  }
  obj1ptr[NUMBER_BLOCKS_ONE] = new Obj1();
  if (obj1ptr[NUMBER_BLOCKS_ONE] != NULL) {
    cout<<"ERROR Could allocate more than req number of blocks pool 1"<<endl;
  }

  cout<<"Testing valid delete"<<endl;
  delete obj3ptr[NUMBER_BLOCKS_THREE - 1];
  obj3ptr[NUMBER_BLOCKS_THREE - 1] = new Obj3();
  if (obj3ptr[NUMBER_BLOCKS_THREE - 1] == NULL) {
    cout<<"ERROR Not deleted properly"<<endl;
  }
  obj3ptr[NUMBER_BLOCKS_THREE] = new Obj3();
  if (obj3ptr[NUMBER_BLOCKS_THREE] != NULL) {
    cout<<"ERROR Could allocate more after delete"<<endl;
  }

  cout<<"Testing invalid delete"<<endl;
  char *tempPtr = ((char *)obj3ptr[NUMBER_BLOCKS_THREE - 1]) + 1;
  obj3ptr[NUMBER_BLOCKS_THREE - 1] = (Obj3 *)tempPtr;
  delete obj3ptr[NUMBER_BLOCKS_THREE - 1];
  if (obj3ptr[NUMBER_BLOCKS_THREE] != NULL) {
    cout<<"ERROR Could allocate more after invalid delete"<<endl;
  }

  return 0;
}
