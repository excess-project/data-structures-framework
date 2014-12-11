// Base class for Producer-Consumer experiments for the experiment framework.
//   Copyright (C) 2014  Anders Gidenstam
//
#pragma once
#include "NBLExperiment.h"

#ifdef USE_NOBLE
#include "Noble.h"
#endif

#ifdef USE_TBB
#include <tbb/concurrent_queue.h>
#endif

// Abstract base class for experiments that use Producer-Consumer collections.
class NBLExpProducerConsumerBase :
  public NBLExperiment
{
public:
  static const int MAX_CPUS = 16;

  NBLExpProducerConsumerBase(void);

  virtual void SetNrThreads(int nrOfThreads);
  virtual vector<string> GetImplementations();

  virtual void InitImplementationNr(int nr);
  // ThreadInitImplementationNr() must be called by each thread before the
  // experiment commences.
  virtual void DeInitImplementationNr(int nr);
  virtual string GetStatistics();
  virtual string GetStatisticsLegend();
protected:
  virtual void SaveThreadStatistics(long countInsert,
                                    long countOkTryRemove,
                                    long countEmptyTryRemove);

  typedef void NBLHandle;
  typedef void (*InsertFunc)(NBLHandle *handle, void *argument, long& count);
  typedef void *(*TryRemoveFunc)(NBLHandle *handle, long& countOk, long& countEmpty);
  typedef void (*FreeHandleFunc)(NBLHandle *handle);

  virtual NBLHandle *ThreadInitImplementationNr(int nr);

  // Experiment shared state.
  int NR_CPUS;
  int NR_OPERS; // For experiments that care about this.

  volatile long long countInsert;
  volatile long long countOkTryRemove;
  volatile long long countEmptyTryRemove;

private:
#ifdef USE_NOBLE
  NBLQueueRoot *queue;
  NBLStackRoot *stack;
  NBLBagRoot   *bag;
#endif
#ifdef USE_TBB
  tbb::concurrent_queue<void *> *tbbqueue;
#endif

protected: 
  // For the currently configured producer-consumer collection. 
  InsertFunc Insert;
  TryRemoveFunc TryRemove;
  FreeHandleFunc FreeHandle;
};
