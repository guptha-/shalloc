#include "shalloclib.hpp"

using namespace shalloclib;

pid_t sharedClass::ownPID = getpid();

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

int main () {
  sharedClass obj;

  obj.test();
}
