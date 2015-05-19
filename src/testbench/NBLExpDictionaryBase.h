// Base class for Dictionary experiments for the experiment framework.
//   Copyright (C) 2015  Anders Gidenstam
//
#pragma once
#include "NBLExperiment.h"

#include <cmath>

#ifdef USE_NOBLE
#include "Noble.h"
#endif

#ifdef USE_TBB
#include <tbb/concurrent_hash_map.h>
#endif

#ifdef USE_ETL
extern "C" {
// Inlined definitions due to header problems.
struct cbtree_t;
cbtree_t* cbtree_alloc();
void* cbtree_free(cbtree_t* map);
int cbtree_insert(cbtree_t* map, void* key, void* data);
int cbtree_contains(cbtree_t* map, void* key);
void *cbtree_get(cbtree_t* map, void* key);
int cbtree_delete(cbtree_t* map, void* key);
}
#endif

#ifdef USE_CCKHT
#include <globaldefinitions.h>
#include <concckhashtable.h>
#include <BucketizeConcCK.h>
#endif

#ifdef USE_HSHT
#define INTEL64 1
#include <HSHashtable.h>
#undef INTEL64
#undef CAS64
#undef CAS32
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
  int MAX_DICTIONARY_SIZE;

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
#ifdef USE_ETL
  cbtree_t* cbsearchtree;
#endif
#ifdef USE_CCKHT
  ConcCukooHashtable<int, void *, HASH_INT, HASH_INT1> *cckhashtable;
  BucketizeConcCK<int, void *>    *bcckhashtable;  
#endif
#ifdef USE_HSHT
  HSBitmapHopscotchHashMap_t *hsbhhashtable;
  HSChainedHashMap_t *hschashtable;
  HSHopscotchHashMap_t *hshhashtable;
#endif

protected: 
  // For the currently configured dictionary collection. 
  InsertFunc Insert;
  TryRemoveFunc Lookup;
  TryRemoveFunc TryRemove;
  FreeHandleFunc FreeHandle;
};
