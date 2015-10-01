// Base class for Producer-Consumer experiments for the experiment framework.
//   Copyright (C) 2014 - 2015  Anders Gidenstam
//
#pragma once
#include "NBLExperiment.h"

#include <EXCESS/concurrent_bag>


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

  typedef excess::concurrent_bag<void> concurrent_bag_t;
  typedef excess::concurrent_bag<void>::handle handle_t;
  typedef void (*InsertFunc)(handle_t *handle, void *argument, long& count);
  typedef void *(*TryRemoveFunc)(handle_t *handle, long& countOk, long& countEmpty);

  virtual handle_t* ThreadInitImplementationNr(int nr);

  // Experiment shared state.
  int NR_CPUS;
  int NR_OPERS; // For experiments that care about this.

  volatile long long countInsert;
  volatile long long countOkTryRemove;
  volatile long long countEmptyTryRemove;

private:
  concurrent_bag_t* bag;

protected: 
  // For the currently configured producer-consumer collection. 
  InsertFunc Insert;
  TryRemoveFunc TryRemove;
};
