#include <iostream>
#include "../lib/shalloclib.hpp"

class Test : public shalloclib::SharedClass {
public:
  Test() {
    cout<<"In child constructor"<<endl;
  }
  int value;
};

void *thrfunc (void *a) {
  cout<<"From thread "<<endl;
  return NULL;
}

int main() {
  Test *obj = new Test();
  obj->value = 5;
  obj->val = 10;

  cout<<obj->value<<"\t"<<obj->val;
  pthread_t thread1;
  int a = 5;

  shalloclib::sthread_create (&thread1, NULL, &thrfunc, (void *) &a);
  return 0;
}
