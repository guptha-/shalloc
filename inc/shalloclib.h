#ifndef SHALLOCLIB_H
#define SHALLOCLIB_H

#include <iostream>
#include <unordered_map>
#include <sys/types.h>
#include <unistd.h>
#include <mutex>
#include <vector>

using namespace std;

namespace shalloclib {
  class SharedClass {
      protected:
      // This is called at the beginning of every setter
    void startWrite();
      // This is called at the end of every setter
    void stopWrite();
      // This is called at the beginning of every getter
    void startRead();
      // This is called at the end of every getter
    void stopRead();
      // This is called in every setter
    void sharedWrite(unsigned int offset, unsigned int size, void *data);

  public:
    static void* operator new (size_t size); 
    static void operator delete (void *p);
    SharedClass();
    ~SharedClass();
    int val;

      // This contains this process's PID
    static pid_t ownPID;

      // The list of processes that share the data structure
    static vector<pid_t> pidList;

    void test();
  };
}
#endif