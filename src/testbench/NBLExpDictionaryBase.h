// Base class for Dictionary experiments for the experiment framework.
//   Copyright (C) 2015  Anders Gidenstam
//
#pragma once
#include "NBLExperiment.h"

#ifdef USE_NOBLE
#include "Noble.h"
#endif

#ifdef USE_TBB
#include <tbb/concurrent_hash_map.h>
#endif

// Abstract base class for experiments that use Dictionary collections.
class NBLExpDictionaryBase :
  public NBLExperiment
{
public:
  static const int MAX_CPUS = 40;

  NBLExpDictionaryBase(void);

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
                                    long countOkLookup,
                                    long countNotFoundLookup,
                                    long countOkTryRemove,
                                    long countNotFoundTryRemove);

  typedef void NBLHandle;
  typedef bool (*InsertFunc)(NBLHandle *handle, int key, void *value,
                             long& count);
  typedef bool (*LookupFunc)(NBLHandle *handle, int key, void*& value,
                             long& countOk, long& countNotFound);
  typedef bool (*TryRemoveFunc)(NBLHandle *handle, int key, void*& value,
                                long& countOk, long& countNotFound);
  typedef void (*FreeHandleFunc)(NBLHandle *handle);

  virtual NBLHandle *ThreadInitImplementationNr(int nr);

  // Experiment shared state.
  int NR_CPUS;

  volatile long long countInsert;
  volatile long long countOkLookup;
  volatile long long countNotFoundLookup;
  volatile long long countOkTryRemove;
  volatile long long countNotFoundTryRemove;

private:
#ifdef USE_NOBLE
  //NBLSkipListRoot *skiplist;
#endif
#ifdef USE_TBB
  tbb::concurrent_hash_map<int, void *> *tbbhashmap;
#endif

protected: 
  // For the currently configured producer-consumer collection. 
  InsertFunc Insert;
  TryRemoveFunc Lookup;
  TryRemoveFunc TryRemove;
  FreeHandleFunc FreeHandle;
};
