// Tests sthread_join
#include <iostream>
#include "../lib/shalloclib.hpp"

using namespace shalloclib;
class Test : public SharedClass {
  public:
  int a;
};

void *fn1(void *arg) {
  usleep(5000);
  cout<<"In child 1"<<endl;
  return NULL;
}

void *fn2(void *arg) {
  cout<<"In child 2"<<endl;
  usleep(20000);
  cout<<"Exiting child 2"<<endl;
  return NULL;
}

int main() {
  cout<<"Starting test case 2"<<endl;
  int a = 5;
  int childPid = sthread_create(fn1, (void *) &a);
  cout<<"Parent: Before join"<<endl;
  usleep(10000);
  sthread_join(childPid);
  cout<<"Parent: Done joining process which was dead before join is called "<<endl;
  childPid = sthread_create(fn2, (void *) &a);
  cout<<"Parent: Before join"<<endl;
  sthread_join(childPid);
  cout<<"Parent: Done joining process which was alive before join called"<<endl;

  return 0;
}
