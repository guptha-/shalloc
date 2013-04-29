#include "shalloclib.hpp"

using namespace shalloclib;

pid_t SharedClass::ownPID = getpid();
vector<pid_t> SharedClass::pidList;
ObjIdLocMap SharedClass::objIdLocMap;
unsigned int SharedClass::maxObjectID(0);
  // Just for creation of subclasses
static mutex createLock;
  // The size of memory allocated. This is a temporary storage, 
  // meaningful between the time memory is allocated by the new operator,
  // and the base class constructor is called. This is used because the baseclass
  // constructor does not know the size allocated for the subclass object.
size_t memAlloc;

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

  // TODO find out why the seg fault occurs
  // typedef unordered_set<int> MyObjIdLocMap;
  // MyObjIdLocMap myObjIdLocMap;
  // int a = 5;
  // int b = 6;

  // pair<MyObjIdLocMap::iterator, bool> notAlreadyExists;
  // notAlreadyExists = myObjIdLocMap.emplace(b);
  // if (get<1>(notAlreadyExists) != true)
  // { 
  //   cout<<"Exists"<<endl;
  // }
  // cout<<"Not Exists"<<endl;


  // while (true == true) {
  //   unsigned int objectID = ++maxObjectID;
  //   this->objLock.lock();
  //   pair<ObjIdLocMap::iterator, bool> notAlreadyExists;
  //   int a = 5;
  //   notAlreadyExists = objIdLocMap.emplace(objectID, &a);
  //   if (get<1>(notAlreadyExists) != true)
  //   {
  //       // The entry already exists
  //     continue;
  //   }
  //   break;
  // }

  objectID = memSize; // TODO
  SharedClass(objectID, getpid());

  // TODO send objectID, pid and memSize to all other processes

}

/* Destructor
 */
SharedClass::~SharedClass() {
  // Send all the other processes the objectID. 
  // We take the locks before we return.

  objLock.lock();
  if (getpid() == ownerPID) {
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