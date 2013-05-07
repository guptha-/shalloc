/*==============================================================================
 *
 *       Filename:  shalloclib.hpp
 *
 *    Description:  The shalloc library
 *
 * =============================================================================
 */

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#define BLOCK_SIZE 128
#define NUMBER_OF_BLOCKS 100

using namespace std;
typedef unsigned int uint;

namespace shalloclib {
  class SharedClass {
    public:
    pthread_mutex_t lock;
    static void* operator new (size_t size);
    static void operator delete (void *p);
    SharedClass();
  };

  void initState();
  int sthread_create (void *(*fn)   (void *), void * arg);
  void sthread_join (int pid);
}
