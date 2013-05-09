/*==============================================================================
 *
 *       Filename:  shalloclib.cpp
 *
 *    Description:  The shalloc library
 *
 * =============================================================================
 */

#include "shalloclib.hpp"

using namespace shalloclib;

/* STATICS */

static MemPool *memPoolOne = new MemPool(BLOCK_SIZE_ONE, NUMBER_BLOCKS_ONE);
static MemPool *memPoolTwo = new MemPool(BLOCK_SIZE_TWO, NUMBER_BLOCKS_TWO);
static MemPool *memPoolThree = new MemPool(BLOCK_SIZE_THREE, NUMBER_BLOCKS_THREE);
/* END STATICS */

/* ===  FUNCTION  ==============================================================
 *         Name:  MemPool
 *  Description:  Constructor for MemPool
 * =============================================================================
 */
MemPool::MemPool (int blockSz, int numberBlks)
{
  blockSize = blockSz;
  numberBlocks = numberBlks;

  static bool firstCall = false;
  if (firstCall == false) {
    firstCall = true;
    srand(time(NULL));
  }
  pthread_mutexattr_t m_attr;
  pthread_mutexattr_init(&m_attr);
  pthread_mutexattr_setpshared(&m_attr, PTHREAD_PROCESS_SHARED);
  pthread_mutex_init(&(lock), &m_attr);
  pthread_mutexattr_destroy(&m_attr);

  int key = rand();
  int size = blockSz * numberBlks + 1;
  char keyStr[20];
  sprintf(keyStr, "%d", key);
  int shmid = shm_open(keyStr, O_RDWR | O_CREAT, 0666);
  if (shmid == -1) {
    cout<<"shm_open failed!"<<endl;
    exit(1);
  }
  ftruncate(shmid, size);
  basePtr = mmap((caddr_t)0, size, PROT_READ | PROT_WRITE, 
                        MAP_SHARED, shmid, 0);
  if (basePtr == (caddr_t)(-1)) {
    cout<<"Error in mmap"<<endl;
    exit(1);
  }
  // Now that we have mmaped it, we can unlink the shared memory object. Once
  // all the processes munmap, the object will be destroyed
  shm_unlink(keyStr);

  // Allocating the status of the heap in the heap
  status = (bool *) basePtr;
  if ((numberBlks * sizeof(bool) > (unsigned int)blockSz)) {
    cout<<"Not enough space in one block to store the bitmap. "
          "Increase block size"<<endl;
    exit(1);
  }
  memset (status, 0, numberBlks * sizeof(bool));
  status[0] = true;

  return;
}		/* -----  end of function MemPool  ----- */

/* ===  FUNCTION  ==============================================================
 *         Name:  getBlock
 *  Description:  Gets a block from the mem pool if possible
 * =============================================================================
 */
void *MemPool::getBlock ()
{
  pthread_mutex_lock(&lock);
  void *ptr = NULL;
  for (int i = 0; i < numberBlocks + 1; i++) {
    if (status[i] == false) {
      // This memory location is unoccupied
      status[i] = true;
      char *tempPtr = (char *)basePtr;
      ptr = (void *) (tempPtr + blockSize * sizeof(bool) * i);
      break;
    }
  }
  if (ptr == NULL) {
    cout<<"The mem pool is full. Try increasing the number of blocks in this "
      "pool with size "<<blockSize<<"!"<<endl;
  }
  pthread_mutex_unlock(&lock);
  return ptr;
}		/* -----  end of function getBlock  ----- */

/* ===  FUNCTION  ==============================================================
 *         Name:  deleteBlock
 *  Description:  Frees up the given block
 * =============================================================================
 */
void MemPool::destroyBlock (void *p)
{
  pthread_mutex_lock(&lock);
  char *tempObjPtr = (char *)p;
  char *tempBasePtr = (char *)basePtr;
  int i = (tempObjPtr - tempBasePtr) / (blockSize * sizeof(bool));
  status[i] = false;
  pthread_mutex_unlock(&lock);
  return;
}		/* -----  end of function deleteBlock  ----- */

/* ===  FUNCTION  ==============================================================
 *         Name:  addrWithinRange
 *  Description:  Checks if the given address is within the purview of this 
 *                mem pool
 * =============================================================================
 */
bool MemPool::addrWithinRange (void *p)
{
  pthread_mutex_lock(&lock);
  char *tempObjPtr = (char *)p;
  char *tempBasePtr = (char *)basePtr;
  int i = (tempObjPtr - tempBasePtr) / (blockSize * sizeof(bool));
  int align = (tempObjPtr - tempBasePtr) % (blockSize * sizeof(bool));
  pthread_mutex_unlock(&lock);
  if ((i < 0) || (i > numberBlocks) || (align != 0)) {
    return false;
  }
  return true;
}		/* -----  end of function addrWithinRange  ----- */

/* ===  FUNCTION  ==============================================================
 *         Name:  SharedClass
 *  Description:  Constructor for shared class
 * =============================================================================
 */
SharedClass::SharedClass ()
{
  pthread_mutexattr_t m_attr;
  pthread_mutexattr_init(&m_attr);
  pthread_mutexattr_setpshared(&m_attr, PTHREAD_PROCESS_SHARED);
  pthread_mutex_init(&(lock), &m_attr);
  pthread_mutexattr_destroy(&m_attr);
  return;
}		/* -----  end of function SharedClass  ----- */

/* ===  FUNCTION  ==============================================================
 *         Name:  operator new
 *  Description:  Override for the new operator for this class and subclasses
 * =============================================================================
 */
void* SharedClass::operator new (size_t size) throw()
{   
  // Find which memPool the block should go into
  void *retPtr = NULL;
  if (size <= BLOCK_SIZE_ONE) {
    retPtr = memPoolOne->getBlock();
  }
  if ((retPtr == NULL) && (size <= BLOCK_SIZE_TWO)) {
    retPtr =  memPoolTwo->getBlock();
  }
  if ((retPtr == NULL) && (size <= BLOCK_SIZE_THREE)) {
    retPtr =  memPoolThree->getBlock();
  }
  if (size > BLOCK_SIZE_THREE) {
    cout<<"Trying to allocate memory larger than maximum block size. Tune that!"
      <<endl;
  }
  return retPtr;
}		/* -----  end of function operator new  ----- */

/* ===  FUNCTION  ==============================================================
 *         Name:  operator delete
 *  Description:  Override for the delete operator for this class and subclasses
 * =============================================================================
 */
void SharedClass::operator delete (void *p) {
  if (memPoolOne->addrWithinRange(p) == true) {
    memPoolOne->destroyBlock(p);
    return;
  }
  if (memPoolTwo->addrWithinRange(p) == true) {
    memPoolTwo->destroyBlock(p);
    return;
  }
  if (memPoolThree->addrWithinRange(p) == true) {
    memPoolThree->destroyBlock(p);
    return;
  }
  cout<<"Trying to delete something not in the shared memory"<<endl;
  return;
}		/* -----  end of function operator delete  ----- */

/* ===  FUNCTION  ==============================================================
 *         Name:  sthread_create
 *  Description:  Should be used to create a new thread that is blown up into
 *                a separate process. Note that entities created with this 
 *                command will not share the heap, and that the heap will be 
 *                copied over to the new entity. Any further changes will not
 *                be reflected to the parent/siblings.
 *                Any object extended from SharedClass will be visible across
 *                all entities created using this function.
 * =============================================================================
 */
int shalloclib::sthread_create (void *(*fn) (void *), void * arg)
{
  int childId = fork();
  if (childId == -1) {
    cout<<"Problem forking!"<<endl;
    exit(1);
  }
  if (childId == 0) {
    // This is the child. Call the given function with the argument
    (*fn)(arg);
    exit(0);
  }
  return childId;
}		/* -----  end of function sthread_create  ----- */

/* ===  FUNCTION  ==============================================================
 *         Name:  sthread_join
 *  Description:  Waits for given process to terminate
 * =============================================================================
 */
void shalloclib::sthread_join (int pid)
{
  waitpid(pid, NULL, 0);
  return;
}		/* -----  end of function sthread_join  ----- */

/* ===  FUNCTION  ==============================================================
 *         Name:  sthread_cancel
 *  Description:  Tries to terminate the process
 * =============================================================================
 */
void shalloclib::sthread_cancel (int pid)
{
  kill(pid, SIGTERM);
}		/* -----  end of function sthread_cancel  ----- */

