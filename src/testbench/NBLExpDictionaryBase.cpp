// Base class for Dictionary experiments for the experiment framework.
//   Copyright (C) 2015  Anders Gidenstam
//
#include "NBLExpDictionaryBase.h"

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

typedef tbb::concurrent_hash_map<int,void*> tbb_hash_map;
#else
#define TBB_STATUS " (unavailable)"
#endif

typedef void NBLHandle;
// Data structures from NOBLE 
#ifdef USE_NOBLE
#endif
// Data structures from TBB.
#ifdef USE_TBB
static bool TBBHashMapInsert(NBLHandle *handle,
                             int key, void *value,
                             long& count);
static bool TBBHashMapLookup(NBLHandle *handle,
                             int key, void*& value,
                             long& countOk, long& countNotFound);
static bool TBBHashMapTryRemove(NBLHandle *handle,
                                int key, void*& value,
                                long& countOk, long& countNotFound);
#endif

#ifdef USE_NOBLE
#endif

#ifdef USE_TBB
static bool TBBHashMapInsert(NBLHandle *handle,
                             int key, void *value,
                             long& count)
{
  bool ret = static_cast<tbb_hash_map*>(handle)->
    insert(std::pair<int,void*>(key,value));
  count++;
  return ret;
}
static bool TBBHashMapLookup(NBLHandle *handle,
                             int key, void*& value,
                             long& countOk, long& countNotFound)
{
  //tbb_hash_map::const_accessor acc;
  if (static_cast<tbb_hash_map*>(handle)->count(key)) {
    countOk++;
    // FIXME: Set the out parameter. Will need to use find().
    return true;
  } else {
    countNotFound++;
    return false;
  }
}
static bool TBBHashMapTryRemove(NBLHandle *handle,
                                int key, void*& value,
                                long& countOk, long& countNotFound)
{
  if (static_cast<tbb_hash_map*>(handle)->erase(key)) {
    countOk++;
    // FIXME: Set the out parameter. Will need to use another erase().
    return true;
  } else {
    countNotFound++;
    return false;
  }
  /*
  tbb_hash_map::const_accessor acc;
  if (static_cast<tbb_hash_map*>(handle)->find(acc, key)) {
    static_cast<tbb_hash_map*>(handle)->erase(acc);
    countOk++;
    // FIXME: Set the out parameter.
    return true;
  } else {
    countNotFound++;
    return false;
  }
  */
}
static void TBBHashMapFreeHandle(void *handle)
{
}
#endif

NBLExpDictionaryBase::NBLExpDictionaryBase(void)
{
  NR_CPUS=2;
}

void NBLExpDictionaryBase::SetNrThreads(int nrOfThreads)
{
  NR_CPUS = std::min(nrOfThreads, NBLExpDictionaryBase::MAX_CPUS);
}

vector<string> NBLExpDictionaryBase::GetImplementations()
{
  vector<string> v;
  v.push_back(string("Dictionary TBB concurrent_hash_map" TBB_STATUS));
  return v;
}

void NBLExpDictionaryBase::InitImplementationNr(int nr)
{
  countInsert = 0;
  countOkLookup = 0;
  countNotFoundLookup = 0;
  countOkTryRemove = 0;
  countNotFoundTryRemove = 0;

  switch(nr) {
#ifdef USE_NOBLE
    //case 0:
    //  break;
#endif
  case 0:
#ifdef USE_TBB
    tbbhashmap = new tbb::concurrent_hash_map<int,void*>();
#else
    std::cerr << "Error: Compiled without Intel TBB support!" << std::endl;
    exit(-1);
#endif
    break;
#ifdef USE_NOBLE
#else
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
  case 1: 
  case 2:
  case 3:
  case 4:
  case 5:
  case 6:
#ifdef USE_NOBLE
#endif
    break;
  case 0:
#ifdef USE_TBB
    Insert = TBBHashMapInsert;
    Lookup = TBBHashMapLookup;
    TryRemove = TBBHashMapTryRemove;
    FreeHandle = TBBHashMapFreeHandle;
#endif
    break;
  case 8:
  case 9:
#ifdef USE_NOBLE
#endif
    break;
  case 10:
  case 11:
#ifdef USE_NOBLE
#endif
    break;
  }
}

NBLHandle *NBLExpDictionaryBase::ThreadInitImplementationNr(int nr)
{
  NBLHandle *handle = 0;
  switch(nr) {
  case 1:
  case 2:
  case 3:
  case 4:
  case 5:
  case 6:
#ifdef USE_NOBLE
#endif
    break;
  case 0:
#ifdef USE_TBB
    handle = tbbhashmap;
#endif
    break;
  case 8:
  case 9:
#ifdef USE_NOBLE
#endif
    break;
  case 10:
  case 11:
#ifdef USE_NOBLE
#endif
    break;
  }
  return handle;
}

void NBLExpDictionaryBase::DeInitImplementationNr(int nr)
{
  switch(nr) {
  case 1:
  case 2:
  case 3:
  case 4:
  case 5:
  case 6:
#ifdef USE_NOBLE
#endif
    break;
  case 0:
#ifdef USE_TBB
    delete tbbhashmap;
#endif
    break;
  case 8:
  case 9:
#ifdef USE_NOBLE
#endif
    break;
  case 10:
  case 11:
#ifdef USE_NOBLE
#endif
    break;
  }
}

string NBLExpDictionaryBase::GetStatistics()
{
  std::stringstream ss;
  ss << countInsert << "  "
     << countOkLookup << " "
     << countNotFoundLookup << " "
     << countOkTryRemove << " "
     << countNotFoundTryRemove;
  return ss.str();
}

string NBLExpDictionaryBase::GetStatisticsLegend()
{
  std::stringstream ss;
  ss << "<#Insert operations>  "
     << "<#Ok Lookup operations>  "
     << "<#NotFound Lookup operations>"
     << "<#Ok TryRemove operations>  "
     << "<#NotFound TryRemove operations>";
  return ss.str();
}

void NBLExpDictionaryBase::SaveThreadStatistics(long countInsert,
                                                long countOkLookup,
                                                long countNotFoundLookup,
                                                long countOkTryRemove,
                                                long countNotFoundTryRemove)
{
  // Update the global operation counters.
  STDFAA(&this->countInsert, countInsert);
  STDFAA(&this->countOkLookup, countOkLookup);
  STDFAA(&this->countNotFoundLookup, countNotFoundLookup);
  STDFAA(&this->countOkTryRemove, countOkTryRemove);
  STDFAA(&this->countNotFoundTryRemove, countNotFoundTryRemove);
}
