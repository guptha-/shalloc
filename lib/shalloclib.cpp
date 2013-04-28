#include "shalloclib.hpp"

using namespace shalloclib;

pid_t sharedClass::ownPID = getpid();
vector<unsigned int> sharedClass::pidList;

/* This is a test for sharedClass
*/
void sharedClass::test() {
  cout<<"Hey there"<<endl;
}

/* This is called at the beginning of every setter. It behaves differently for
 * owners and for other processes.
 */
void sharedClass::startWrite() {
  objLock.lock();
  if (ownerPID == sharedClass::ownPID) {
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
void sharedClass::stopWrite() {
  if (ownerPID == sharedClass::ownPID) {
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
void sharedClass::startRead() {
  objLock.lock();
}

/* This is called at the end of every getter.
 */
void sharedClass::stopRead() {
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
void sharedClass::sharedWrite(unsigned int offset, unsigned int size, 
                              void *data) {
  for (int i = 0, n = pidList.size(); i < n; i++) {
    // TODO send to every other process on the list the objectID, the offset, 
    // the size and the data.
  }
}

/* This function obtains the lock for this object at the owner
 */
void sharedClass::getLockAtOwner() {
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
void sharedClass::releaseLockAtOwner() {
    responseLock.lock();
  // Send a release lock message to the owner. We need to send the object ID to the
  // port that the owner is listening at
  
  // TODO

  // We try to take the lock here again. The lock would have been released when
  // the owner responds
  responseLock.lock();
  responseLock.unlock();
}

int main () {
  sharedClass obj;

  obj.test();
}
