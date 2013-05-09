// Tests the sharing of objects amongst processes. 
#include <iostream>
#include "../lib/shalloclib.hpp"

using namespace shalloclib;

class Obj : public SharedClass {
  public:
  int a;
  Obj *obj;
};

static Obj *obj;

void *fn1(void *arg) {
  cout<<"In child 1 "<<obj->a<<endl;
  obj->obj = new Obj();
  obj->obj->a = 6;
  pthread_mutex_lock(&obj->obj->lock);
  pthread_mutex_unlock(&obj->lock);

  return NULL;
}

void *fn2(void *arg) {
  pthread_mutex_lock(&obj->lock);
  pthread_mutex_unlock(&obj->lock);
  cout<<"In child 2 "<<obj->obj->a<<endl;
  obj->obj->obj = new Obj();
  obj->obj->obj->a = 7;
  pthread_mutex_unlock(&obj->obj->lock);
  return NULL;
}

void *fn3(void *arg) {
  cout<<"In child 3 "<<obj->obj->obj->a<<endl;
  return NULL;
}

int main() {
  cout<<"Starting test case 6"<<endl;
  obj = new Obj();
  obj->a = 5;

  int a = 3;
  pthread_mutex_lock(&obj->lock);
  sthread_create(fn1, (void *) &a);
  sthread_create(fn2, (void *) &a);
  usleep(10000);
  sthread_create(fn3, (void *) &a);

  return 0;
}
