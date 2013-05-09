// Tests sthread_cancel
#include <iostream>
#include "../lib/shalloclib.hpp"

using namespace shalloclib;
class Test : public SharedClass {
  public:
  int a;
};

void *fn1(void *arg) {
  cout<<"In child 1"<<endl;
  usleep(50000);
  cout<<"ERROR In child 1 even after cancel"<<endl;
  return NULL;
}

int main() {
  cout<<"Starting test case 4"<<endl;
  int a = 5;
  int childPid = sthread_create(fn1, (void *) &a);
  usleep(10000);
  cout<<"Parent: Before cancel"<<endl;
  sthread_cancel(childPid);
  cout<<"Parent: Done canceling process "<<endl;

  return 0;
}
