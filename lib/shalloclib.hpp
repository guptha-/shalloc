#ifndef SHALLOCLIB_HPP
#define SHALLOCLIB_HPP

#include <iostream>
#include <unordered_map>
#include <sys/types.h>
#include <unistd.h>
#include <mutex>
#include <vector>
#include <ctime>
#include <atomic>
#include <csignal>
#include <syscall.h>
using namespace std;

namespace shalloclib {

  typedef unordered_map<unsigned int, void *> ObjIdLocMap;
  typedef unordered_map<pid_t, unsigned short int> PidPortMap;

  class SharedClass {
      // This contains the owner process ID of the shared DS
    pid_t ownerPID;

      // This defines the ID of this particular object. It is common
      // across all the other processes. Initially, when the object is
      // created in another process, we won't have any idea about what
      // its type is. This object ID helps uniquely identify it for modification
    unsigned int objectID;

      // This is the lock that has to be taken before trying to write to the DS.
      // This is meaningful only in the owner
    mutex objCommonLock;

      // This is the local lock. This is taken before reading and writing. This
      // does not have global significance, but prevents us from reading when
      // the fields are being updated in the local copy. In the owner, for a 
      // local write, the common lock is taken after this. So, while a remote 
      // update is taking place, local reads cannot take place.
    mutex objLock;

      // This is pretty unusual. When we attempt to obtain the write lock
      // from the owner, we have to wait for the response. The response is 
      // gathered in a separate thread. Till the response is received from the
      // owner, we wait on this lock
    mutex responseLock;

      // Indicates whether the owner granted the lock or not
    bool responseStatus;

      // The holder of the common lock
    pid_t holderPID;

      // Maximum known objectID
    static unsigned int maxObjectID;

      // Size of the derived object
    unsigned int objectSize;

    void getLockAtOwner();
    void releaseLockAtOwner();
    SharedClass(unsigned int objectID, pid_t ownerPID);

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

    static ObjIdLocMap objIdLocMap;

    void test();
  };
  int pthread_create (pthread_t * tid, const pthread_attr_t * attr, 
                      void *(*fn)   (void *), void * arg);
}

#endif