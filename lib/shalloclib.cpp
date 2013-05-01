#include "shalloclib.hpp"

using namespace shalloclib;

// STATIC VARIABLES========================================
  // Maximum known objectID
atomic<unsigned int> SharedClass::maxObjectID;
  // This contains this process's PID
pid_t SharedClass::ownPID;
  // This is the own port ID on which the process listens
unsigned int SharedClass::portID;
  // The list of processes that share the data structure
vector<pid_t> SharedClass::pidList;
ObjIdLocMap SharedClass::objIdLocMap;

atomic<unsigned int> ThreadArg::maxAssignedPort;

// END STATIC VARIABLES====================================

  // Just for creation of subclasses
static mutex createLock;
  // The size of memory allocated. This is a temporary store, 
  // meaningful between the time memory is allocated by the new operator,
  // and the base class constructor is called. This is used because the baseclass
  // constructor does not know the size allocated for the subclass object.
size_t memAlloc;

  // Just for waiting till listen thread is initialized
static mutex listenLock;


/* Constructor
 * @param objectID the unique ID for this particular object
 * @param ownerPID the process ID of the owner of this object
 */
SharedClass::SharedClass(unsigned int objectID, pid_t ownerPID) {
  this->objectID = objectID;
  this->ownerPID = ownerPID;
}

/* Constructor
 */
SharedClass::SharedClass() {
  cout<<"In parent's constructor"<<endl;

  size_t memSize = memAlloc;
  // Other creations can proceed
  createLock.unlock();

  while (true == true) {
    unsigned int objectID = ++SharedClass::maxObjectID;
    this->objLock.lock();
    pair<ObjIdLocMap::iterator, bool> notAlreadyExists;
    int a = 5;
    notAlreadyExists = objIdLocMap.emplace(objectID, &a);
    if (get<1>(notAlreadyExists) != true)
    {
        // The entry already exists
      continue;
    }
    break;
  }

  objectSize = memSize;
  SharedClass(objectID, SharedClass::ownPID);

  // TODO send objectID, pid and memSize to all other processes

}

/* Destructor
 */
SharedClass::~SharedClass() {
  // Send all the other processes the objectID. 
  // We take the locks before we return.

  objLock.lock();
  if (ownPID == ownerPID) {
    objCommonLock.lock();
  }
}

/* Override for new
 */
void* SharedClass::operator new (size_t size)
{
    // We take this lock, and release it in the constructor
  createLock.lock();
  memAlloc = size;
  SharedClass *ptr = (SharedClass *) malloc(size);
  return ptr;
}

/* Override for delete. Straightforward
 */
void SharedClass::operator delete (void *p) {
  free(p);
}

/* This is called at the beginning of every setter. It behaves differently for
 * owners and for other processes.
 */
void SharedClass::startWrite() {
  objLock.lock();
  if (ownerPID == SharedClass::ownPID) {
    // We are the owner of this object
    objCommonLock.lock();
  } else {
    // We need to obtain the lock from the owner
    while (true == true) {
      getLockAtOwner();
      if (responseStatus == true) {
        break;
      }
    }
  }
}

/* This is called at the end of every setter. It behaves differently for
 * owners and for other processes.
 */
void SharedClass::stopWrite() {
  if (ownerPID == SharedClass::ownPID) {
    // We are the owner of this object
    objCommonLock.unlock();
  } else {
    // We need to release the lock at the owner
    releaseLockAtOwner();
  }
  objLock.unlock();
}

/* This is called at the beginning of every getter.
 */
void SharedClass::startRead() {
  objLock.lock();
}

/* This is called at the end of every getter.
 */
void SharedClass::stopRead() {
  objLock.unlock();
}

/* This is called within every setter. It is assumed that the required locks 
 * have already been taken. The function updates every other copy with the 
 * update
 * @param offset The offset of the current item from the beginning of the
 *               data structure
 * @param size   The size of the item in bytes
 * @param data   The actual value to be stored in the item
 */
void SharedClass::sharedWrite(unsigned int offset, unsigned int size, 
                              void *data) {
  for (int i = 0, n = pidList.size(); i < n; i++) {
    // TODO send to every other process on the list the objectID, the offset, 
    // the size and the data.
  }
}

/* This function obtains the lock for this object at the owner
 */
void SharedClass::getLockAtOwner() {
  responseLock.lock();
  // Send a get lock message to the owner. We need to send the object ID to the
  // port that the owner is listening at
  
  // TODO

  // We try to take the lock here again. The lock would have been released when
  // the owner responds
  responseLock.lock();
  responseLock.unlock();
}

/* This function releases the lock for this object at the owner
 */
void SharedClass::releaseLockAtOwner() {
    responseLock.lock();
  // Send a release lock message to the owner. We need to send the object ID to the
  // port that the owner is listening at
  
  // TODO

  // We try to take the lock here again. The lock would have been released when
  // the owner responds
  responseLock.lock();
  responseLock.unlock();
}

int shalloclib::sthread_create (pthread_t * tid, const pthread_attr_t * attr, 
                    void *(*fn)   (void *), void * arg) {
  cout<<"In sthread_create"<<endl;
  // TODO manipulate arg to assign port ID

  int child = syscall(SYS_clone, CLONE_FS | CLONE_FILES | SIGCHLD, (void*) 0);
  
  if (child) {
    cout<<"Child"<<endl;
    return 0;
  } else {
   cout<<"Parent"<<endl;
   return 0;
 }
}

/* This function should be called as soon as the program begins
 * execution, before doing anything with SharedClass or creating
 * threads.
 */
void shalloclib::initState() {
  SharedClass::initState();
  srand(time(NULL));
  ThreadArg::maxAssignedPort = 10000;
}

/* Initialization for SharedClass statics
 */
void SharedClass::initState() {
  SharedClass::ownPID = getpid();
  SharedClass::maxObjectID = 0;
  SharedClass::portID = ThreadArg::maxAssignedPort++;
  pthread_t listThread;
  cout<<"Before pthread_create"<<endl;
  listenLock.lock();
  pthread_create(&listThread, NULL, &listenerFlow, (void *) SharedClass::portID);
  pthread_detach(listThread);
  listenLock.lock();
  listenLock.unlock();
  cout<<"after final unlock"<<endl;
}

void *shalloclib::listenerFlow(void *arg) {
  cout<<"listener flow"<<endl;
  UDPSocket listenSocket ((unsigned int) arg);

  while (true) {
    // Block for msg receipt
    int inMsgSize;
    char *inMsg;
    inMsg = new char[1000]();
    try {
      cout<<"before recv unlock"<<endl;
      listenLock.unlock();
      cout<<"on recv"<<endl;
      inMsgSize = listenSocket.recv(inMsg, 1000);
      cout<<"after recv"<<endl;
    } catch (SocketException &e) {
      cout<<"Process "<<e.what()<<endl;
      return NULL;
    }
    inMsg[inMsgSize] = '\0';
  }
  return NULL;
}