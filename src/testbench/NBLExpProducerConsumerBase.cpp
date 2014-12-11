// Base class for Producer-Consumer experiments for the experiment framework.
//   Copyright (C) 2014  Anders Gidenstam
//
#include "NBLExpProducerConsumerBase.h"

#include <sstream>
#include <iostream>

#include <cstdlib>

#include "primitives.h"

#ifdef USE_NOBLE
#define NOBLE_STATUS
#else
#define NOBLE_STATUS " (unavailable)"
#endif

#ifdef USE_TBB
#define TBB_STATUS
#else
#define TBB_STATUS " (unavailable)"
#endif

typedef void NBLHandle;
// Data structures from NOBLE 
#ifdef USE_NOBLE
static void QueueInsert(NBLHandle *handle, void *argument, long& count);
static void *QueueTryRemove(NBLHandle *handle, long& countOk, long& countEmpty);
static void QueueFreeHandle(NBLHandle *handle);
static void StackInsert(NBLHandle *handle, void *argument, long& count);
static void *StackTryRemove(NBLHandle *handle, long& countOk, long& countEmpty);
static void StackFreeHandle(NBLHandle *handle);
static void BagInsert(NBLHandle *handle, void *argument, long& count);
static void *BagTryRemove(NBLHandle *handle, long& countOk, long& countEmpty);
static void BagFreeHandle(NBLHandle *handle);
#endif
// Data structures from TBB.
#ifdef USE_TBB
static void TBBQueueInsert(void* queue, void *argument, long& count);
static void *TBBQueueTryRemove(void* queue, long& countOk, long& countEmpty);
static void TBBQueueFreeHandle(NBLHandle *handle);
#endif

#ifdef USE_NOBLE
static void QueueInsert(NBLHandle *handle, void *argument, long& count)
{
  NBLQueueEnqueue(static_cast<NBLQueue*>(handle), argument);
  count++;
}
static void *QueueTryRemove(NBLHandle *handle, long& countOk, long& countEmpty)
{
  void* res = NBLQueueDequeue(static_cast<NBLQueue*>(handle));
  if (res) {
    countOk++;
  } else {
    countEmpty++;
  }
  return res;
}
static void QueueFreeHandle(NBLHandle *handle)
{
  NBLQueueFreeHandle(static_cast<NBLQueue*>(handle));
}

static void StackInsert(void *handle, void *argument, long& count)
{
  NBLStackPush(static_cast<NBLStack*>(handle), argument);
  count++;
}
static void *StackTryRemove(void *handle, long& countOk, long& countEmpty)
{
  void* res = NBLStackPop(static_cast<NBLStack*>(handle));
  if (res) {
    countOk++;
  } else {
    countEmpty++;
  }
  return res;
}
static void StackFreeHandle(NBLHandle *handle)
{
  NBLStackFreeHandle(static_cast<NBLStack*>(handle));
}

static void BagInsert(void *handle, void *argument, long& count)
{
  NBLBagAdd(static_cast<NBLBag*>(handle), argument);
  count++;
}
static void *BagTryRemove(void *handle, long& countOk, long& countEmpty)
{
  void* res = NBLBagTryRemoveAny(static_cast<NBLBag*>(handle));
  if (res) {
    countOk++;
  } else {
    countEmpty++;
  }
  return res;
}
static void BagFreeHandle(NBLHandle *handle)
{
  NBLBagFreeHandle(static_cast<NBLBag*>(handle));
}
#endif

#ifdef USE_TBB
static void TBBQueueInsert(void *handle, void *argument, long& count)
{
  static_cast<tbb::concurrent_queue<void*>*>(handle)->push(argument);
  count++;
}
static void *TBBQueueTryRemove(void *handle, long& countOk, long& countEmpty)
{
  void *item;
  bool res =
    static_cast<tbb::concurrent_queue<void*>*>(handle)->try_pop(item);
  if (res) {
    countOk++;
    return item;
  } else {
    countEmpty++;
    return 0;
  }
}
static void TBBQueueFreeHandle(void *handle)
{
}
#endif

NBLExpProducerConsumerBase::NBLExpProducerConsumerBase(void)
{
  NR_CPUS=2;
  NR_OPERS=500000;
}

void NBLExpProducerConsumerBase::SetNrThreads(int nrOfThreads)
{
  NR_CPUS = std::min(nrOfThreads, NBLExpProducerConsumerBase::MAX_CPUS);
}

vector<string> NBLExpProducerConsumerBase::GetImplementations()
{
  vector<string> v;
  v.push_back(string("Queue NOBLE Lock-Free DB" NOBLE_STATUS));
  v.push_back(string("Queue NOBLE Lock-Free DU" NOBLE_STATUS));
  v.push_back(string("Queue NOBLE Lock-Free SB" NOBLE_STATUS));
  v.push_back(string("Queue NOBLE Lock-Free BB" NOBLE_STATUS));
  v.push_back(string("Queue NOBLE Lock-Free BASKET" NOBLE_STATUS));
  v.push_back(string("Queue NOBLE Lock-Free ELIM" NOBLE_STATUS));
  v.push_back(string("Queue NOBLE Lock-Based" NOBLE_STATUS));
  v.push_back(string("Queue TBB concurrent_queue" TBB_STATUS));
  v.push_back(string("Stack NOBLE Lock-Free B" NOBLE_STATUS));
  v.push_back(string("Stack NOBLE Lock-Free ELIM" NOBLE_STATUS));
  v.push_back(string("Bag NOBLE Lock-Free BB" NOBLE_STATUS));
  v.push_back(string("Pool NOBLE Lock-Free EDTREE" NOBLE_STATUS));
  return v;
}

void NBLExpProducerConsumerBase::InitImplementationNr(int nr)
{
  countInsert = 0;
  countOkTryRemove = 0;
  countEmptyTryRemove = 0;

  switch(nr) {
#ifdef USE_NOBLE
  case 0:
    queue=NBLQueueCreateLF_DB(NR_OPERS*NBLExpProducerConsumerBase::MAX_CPUS);
    break;
  case 1:
    queue=NBLQueueCreateLF_DU(NR_OPERS*NBLExpProducerConsumerBase::MAX_CPUS);
    break;
  case 2:
    queue=NBLQueueCreateLF_SB(NR_OPERS*NBLExpProducerConsumerBase::MAX_CPUS);
    break;
  case 3:
    queue=NBLQueueCreateLF_BB(NR_OPERS*NBLExpProducerConsumerBase::MAX_CPUS);
    break;
  case 4:
    queue=NBLQueueCreateLF_BASKET(NR_OPERS*NBLExpProducerConsumerBase::MAX_CPUS);
    break;
  case 5:
    queue=NBLQueueCreateLF_ELIM(NR_OPERS*NBLExpProducerConsumerBase::MAX_CPUS);
    break;
  case 6:
    queue=NBLQueueCreateLB();
    break;
#endif
  case 7:
#ifdef USE_TBB
    tbbqueue = new tbb::concurrent_queue<void*>();
#else
    std::cerr << "Error: Compiled without Intel TBB support!" << std::endl;
    exit(-1);
#endif
    break;
#ifdef USE_NOBLE
  case 8:
    stack=NBLStackCreateLF_B(NR_OPERS*NBLExpProducerConsumerBase::MAX_CPUS);
    break;
  case 9:
    stack=NBLStackCreateLF_ELIM(NR_OPERS*NBLExpProducerConsumerBase::MAX_CPUS);
    break;
  case 10:
    bag=NBLBagCreateLF_BB(NR_OPERS*NBLExpProducerConsumerBase::MAX_CPUS);
    break;
  case 11:
    bag=NBLBagCreateLF_EDTREE(NR_OPERS*NBLExpProducerConsumerBase::MAX_CPUS);
    break;
#else
  case 0:
  case 1:
  case 2:
  case 3:
  case 4:
  case 5:
  case 6:
  case 8:
  case 9:
  case 10:
  case 11:
    std::cerr << "Error: Compiled without NOBLE support!" << std::endl;
    exit(-1);
    break;
#endif
  }
  // Set up the collection operations pointers.
  switch(nr) {
  case 0:    
  case 1: 
  case 2:
  case 3:
  case 4:
  case 5:
  case 6:
#ifdef USE_NOBLE
    Insert = QueueInsert;
    TryRemove = QueueTryRemove;
    FreeHandle = QueueFreeHandle;
#endif
    break;
  case 7:
#ifdef USE_TBB
    Insert = TBBQueueInsert;
    TryRemove = TBBQueueTryRemove;
    FreeHandle = TBBQueueFreeHandle;
#endif
    break;
  case 8:
  case 9:
#ifdef USE_NOBLE
    Insert = StackInsert;
    TryRemove = StackTryRemove;
    FreeHandle = StackFreeHandle;
#endif
    break;
  case 10:
  case 11:
#ifdef USE_NOBLE
    Insert = BagInsert;
    TryRemove = BagTryRemove;
    FreeHandle = BagFreeHandle;
#endif
    break;
  }
}

NBLHandle *NBLExpProducerConsumerBase::ThreadInitImplementationNr(int nr)
{
  NBLHandle *handle = 0;
  switch(nr) {
  case 0:    
  case 1: 
  case 2:
  case 3:
  case 4:
  case 5:
  case 6:
#ifdef USE_NOBLE
    handle = NBLQueueGetHandle(queue);
#endif
    break;
  case 7:
#ifdef USE_TBB
    handle = tbbqueue;
#endif
    break;
  case 8:
  case 9:
#ifdef USE_NOBLE
    handle = NBLStackGetHandle(stack);
#endif
    break;
  case 10:
  case 11:
#ifdef USE_NOBLE
    handle = NBLBagGetHandle(bag);
#endif
    break;
  }
  return handle;
}

void NBLExpProducerConsumerBase::DeInitImplementationNr(int nr)
{
  switch(nr) {
  case 0:    
  case 1:
  case 2:
  case 3:
  case 4:
  case 5:
  case 6:
#ifdef USE_NOBLE
    NBLQueueFree(queue);
#endif
    break;
  case 7:
#ifdef USE_TBB
    delete tbbqueue;
#endif
    break;
  case 8:
  case 9:
#ifdef USE_NOBLE
    NBLStackFree(stack);
#endif
    break;
  case 10:
  case 11:
#ifdef USE_NOBLE
    NBLBagFree(bag);
#endif
    break;
  }
}

string NBLExpProducerConsumerBase::GetStatistics()
{
  std::stringstream ss;
  ss << countInsert << "  "
     << countOkTryRemove << "  "
     << countEmptyTryRemove;
  return ss.str();
}

string NBLExpProducerConsumerBase::GetStatisticsLegend()
{
  std::stringstream ss;
  ss << "<#Insert operations>  "
     << "<#Non-empty TryRemove operations>  "
     << "<#Empty TryRemove operations>";
  return ss.str();
}

void NBLExpProducerConsumerBase::SaveThreadStatistics(long countInsert,
                                                      long countOkTryRemove,
                                                      long countEmptyTryRemove)
{
  // Update the global operation counters.
  STDFAA(&this->countInsert, countInsert);
  STDFAA(&this->countOkTryRemove, countOkTryRemove);
  STDFAA(&this->countEmptyTryRemove, countEmptyTryRemove);
}
