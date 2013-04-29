#include <iostream>
#include "../inc/shalloclib.h"

class Test : public shalloclib::SharedClass {
public:
  Test() {
    cout<<"In child constructor"<<endl;
  }
  int value;
};

int main() {
  Test *obj = new Test();
  obj->value = 5;
  obj->val = 10;

  cout<<obj->value<<"\t"<<obj->val;
  return 0;

}