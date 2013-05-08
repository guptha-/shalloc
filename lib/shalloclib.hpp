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

#define BLOCK_SIZE_ONE 64
#define NUMBER_BLOCKS_ONE 20
#define BLOCK_SIZE_TWO 128
#define NUMBER_BLOCKS_TWO 20
#define BLOCK_SIZE_THREE 512
#define NUMBER_BLOCKS_THREE 20

using namespace std;
typedef unsigned int uint;

namespace shalloclib {
  class SharedClass {
    public:
    pthread_mutex_t lock;
    static void* operator new (size_t size) throw();
    static void operator delete (void *p);
    SharedClass();
  };
  
  class MemPool {
    int blockSize;
    int numberBlocks;
    void *basePtr;
    bool *status;
    pthread_mutex_t lock;
    public:
    MemPool(int blockSz, int numberBlks);
    void *getBlock();
    void destroyBlock(void *);
    bool addrWithinRange(void *);
  };

  void initState();
  int sthread_create (void *(*fn)   (void *), void * arg);
  void sthread_join (int pid);
  void sthread_cancel (int pid);
}
