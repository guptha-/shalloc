#include <iostream>
#include "../lib/shalloclib.hpp"

using namespace shalloclib;
class Test : public SharedClass {
  public:
  int a;
};

class Obj : public SharedClass {
  public:
  Test *obj1;
  Test *obj2;
  Test *obj3;
};

static Obj *obj;

void *fn(void *arg) {
  // B: Waiting on lock till parent releases it
  pthread_mutex_lock(&obj->obj1->lock);
  int b = *(int *)arg;
  cout<<"Child: Arg is "<<b<<endl;
  cout<<"Child: Obj 1->a "<<obj->obj1->a<<endl;
  cout<<"Child: Obj 2->a "<<obj->obj2->a<<endl;
  // D: Now make parent wait till obj2 is updated
  pthread_mutex_lock(&obj->obj2->lock);
  obj->obj2->a = 8;
  obj->obj3 = new Test();
  cout<<"IGNORE Child: Obj 3 memory: "<<obj->obj3<<endl;
  obj->obj3->a = 11;
  // E: Now let parent read obj2
  pthread_mutex_unlock(&obj->obj2->lock);
  usleep(20000);
  pthread_mutex_lock(&obj->obj3->lock);
  cout<<"Child: Obj 3->a "<<obj->obj3->a<<endl;
  pthread_mutex_unlock(&obj->obj3->lock);
  return NULL;
}

int main() {
  initState();
  cout<<"Starting test case 1"<<endl;
  obj = new Obj();
  obj->obj1 = new Test();
  cout<<"IGNORE Parent: Obj 1 memory: "<<obj->obj1<<endl;
  obj->obj2 = new Test();
  cout<<"IGNORE Parent: Obj 2 memory: "<<obj->obj2<<endl;
  obj->obj1->a = 5;
  obj->obj2->a = 6;

  cout<<"Parent: Obj 1->a "<<obj->obj1->a<<endl;
  cout<<"Parent: Obj 2->a "<<obj->obj2->a<<endl;

  int a = 3;
  
  // A: Lock obj1, making the child wait till released
  pthread_mutex_lock(&obj->obj1->lock);
  int childPid = sthread_create(fn, (void *) &a);
  usleep(10000);
  obj->obj1->a = 7;
  // C: Release lock so that child can now access obj1 updated
  pthread_mutex_unlock(&obj->obj1->lock);
  usleep(10000);
  // F: Wait till obj2 updated by the child
  pthread_mutex_lock(&obj->obj2->lock);
  cout<<"Parent: Obj 2->a "<<obj->obj2->a<<endl;
  cout<<"Parent: Obj 3->a "<<obj->obj3->a<<endl;
  pthread_mutex_unlock(&obj->obj2->lock);
  pthread_mutex_lock(&obj->obj3->lock);
  cout<<"Parent: updating obj3"<<endl;
  obj->obj3->a = 12;
  pthread_mutex_unlock(&obj->obj3->lock);

  return 0;
}
