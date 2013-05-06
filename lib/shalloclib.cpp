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
static bool *memPoolBitmap;
static void *memPoolBasePtr;
static mutex memPoolLock;
/* END STATICS */

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
  pthread_mutex_init(&(this->lock), &m_attr);
  pthread_mutexattr_destroy(&m_attr);
  return;
}		/* -----  end of function SharedClass  ----- */

/* ===  FUNCTION  ==============================================================
 *         Name:  operator new
 *  Description:  Override for the new operator for this class and subclasses
 * =============================================================================
 */
void* SharedClass::operator new (size_t size)
{   
  memPoolLock.lock();
  if (size > BLOCK_SIZE) {
    cout<<"Trying to allocate memory larger than BLOCK_SIZE. Tune that!"<<endl;
    memPoolLock.unlock();
  }
  void *ptr = NULL;
  for (int i = 0; i < NUMBER_OF_BLOCKS; i++) {
    if (memPoolBitmap[i] == false) {
      // This memory location is unoccupied
      memPoolBitmap[i] = true;
      char *tempPtr = (char *)memPoolBasePtr;
      ptr = (void *) (tempPtr + BLOCK_SIZE * i);
      break;
    }
  }
  if (ptr == NULL) {
    cout<<"The mem pool is full. Try increasing NUMBER_OF_BLOCKS!"<<endl;
  }
  memPoolLock.unlock();
  return ptr;
}		/* -----  end of function operator new  ----- */

/* ===  FUNCTION  ==============================================================
 *         Name:  operator delete
 *  Description:  Override for the delete operator for this class and subclasses
 * =============================================================================
 */
void SharedClass::operator delete (void *p) {
  memPoolLock.lock();
  char *tempObjPtr = (char *)p;
  char *tempBasePtr = (char *)memPoolBasePtr;
  int i = (tempObjPtr - tempBasePtr) / BLOCK_SIZE;
  memPoolBitmap[i] = false;
  memPoolLock.unlock();
}		/* -----  end of function operator delete  ----- */

/* ===  FUNCTION  ==============================================================
 *         Name:  initState
 *  Description:  Should be called as soon as the program starts up, before
 *                trying to create threads or extending SharedClass. Sets up the
 *                shared memory pool.
 * =============================================================================
 */
void shalloclib::initState ()
{
  srand(time(NULL));
  int key = rand();
  int size = BLOCK_SIZE * NUMBER_OF_BLOCKS;
  char keyStr[20];
  sprintf(keyStr, "%d", key);
  int shmid = shm_open(keyStr, O_RDWR | O_CREAT, 0666);
  if (shmid == -1) {
    cout<<"shm_open failed!"<<endl;
    exit(1);
  }
  ftruncate(shmid, size);
  memPoolBasePtr = mmap((caddr_t)0, size, PROT_READ | PROT_WRITE, 
                            MAP_SHARED, shmid, 0);
  if (memPoolBasePtr == (caddr_t)(-1)) {
    cout<<"Error in mmap"<<endl;
    exit(1);
  }

  // Allocating the bitmap of the bitmap in the bitmap
  memPoolBitmap = (bool *)memPoolBasePtr;
  if (NUMBER_OF_BLOCKS / 8 + 1 > BLOCK_SIZE) {
    cout<<"Not enough space in one block to store the bitmap. "
          "Increase BLOCK_SIZE"<<endl;
    exit(1);
  }
  memset (memPoolBitmap, 0, NUMBER_OF_BLOCKS / 8 + 1);
  memPoolBitmap[0] = true;
  cout<<"Mempoolbitmap "<<memPoolBitmap<<endl;
  return;
}		/* -----  end of function initState  ----- */

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
  if (childId != 0) {
    // This is the child. Call the given function with the argument
    cout<<"In child"<<endl;
    (*fn)(arg);
    exit(0);
  }
  return childId;
}		/* -----  end of function sthread_create  ----- */
