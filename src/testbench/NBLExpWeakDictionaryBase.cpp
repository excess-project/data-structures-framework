// Base class for weak dictionary experiments for the experiment framework.
//   Copyright (C) 2015  Anders Gidenstam
//
#include "NBLExpWeakDictionaryBase.h"

#include <sstream>
#include <iostream>

#include <cstdlib>

#include "primitives.h"

#ifdef USE_NOBLE
#define NOBLE_STATUS " (unavailable)"
#else
#define NOBLE_STATUS " (unavailable)"
#endif

#ifdef USE_TBB
#define TBB_STATUS
#else
#define TBB_STATUS " (unavailable)"
#endif

#ifdef USE_ETL
#define ETL_STATUS
#else
#define ETL_STATUS " (unavailable)"
#endif

#ifdef USE_CCKHT
#define CCKHT_STATUS
#else
#define CCKHT_STATUS " (unavailable)"
#endif

#ifdef USE_HSHT
#define HSHT_STATUS
#else
#define HSHT_STATUS " (unavailable)"
#endif

NBLExpWeakDictionaryBase::NBLExpWeakDictionaryBase(void)
{
  NR_CPUS=2;
}

void NBLExpWeakDictionaryBase::SetNrThreads(int nrOfThreads)
{
  NR_CPUS = std::min(nrOfThreads, NBLExpWeakDictionaryBase::MAX_CPUS);
}

vector<string> NBLExpWeakDictionaryBase::GetImplementations()
{
  vector<string> v;
  v.push_back(string("Dictionary NOBLE SkipList" NOBLE_STATUS));
  v.push_back(string("Dictionary TBB concurrent_hash_map" TBB_STATUS));
  v.push_back(string("Dictionary ETL cbtree_t" ETL_STATUS));
  v.push_back(string("Dictionary CCKHT Concurrent Cuckoo hash table"
                     CCKHT_STATUS));
  v.push_back(string("Dictionary CCKHT Bucketized Concurrent Cuckoo hash table"
                     CCKHT_STATUS));
  v.push_back(string("Dictionary HS bitmap hopscotch hash table"
                     HSHT_STATUS));
  v.push_back(string("Dictionary HS chained hash table"
                     HSHT_STATUS));
  v.push_back(string("Dictionary HS hopscotch hash table"
                     HSHT_STATUS));
  return v;
}

void NBLExpWeakDictionaryBase::InitImplementationNr(int nr)
{
  countInsert = 0;
  countOkLookup = 0;
  countNotFoundLookup = 0;
  countRemove = 0;

  switch(nr) {
  case 0:
#ifdef USE_NOBLE
#else
#endif
    std::cerr << "Error: Compiled without NOBLE support!" << std::endl;
    exit(-1);
    break;
  case 1:
#ifdef USE_TBB
    weak_dictionary =
      new excess::concurrent_weak_dictionary_TBBhashmap<int,void>();
#else
    std::cerr << "Error: Compiled without Intel TBB support!" << std::endl;
    exit(-1);
#endif
    break;
  case 2:
#ifdef USE_ETL
    weak_dictionary =
      new excess::concurrent_weak_dictionary_CBTree<int,void>();
#else
    std::cerr << "Error: Compiled without EXCESS Tree Library support!"
              << std::endl;
    exit(-1);
#endif
    break;
  case 3:
#ifdef USE_CCKHT
    weak_dictionary =
      new excess::concurrent_weak_dictionary_CCKHT<int,void>(MAX_DICTIONARY_SIZE);
#else
    std::cerr << "Error: Compiled without CCKHT support!" << std::endl;
    exit(-1);
#endif
    break;
  case 4:
#ifdef USE_CCKHT
    weak_dictionary =
      new excess::concurrent_weak_dictionary_BCCKHT<int,void>(MAX_DICTIONARY_SIZE);
#else
    std::cerr << "Error: Compiled without CCKHT support!" << std::endl;
    exit(-1);
#endif
    break;
  case 5:
#ifdef USE_HSHT
    weak_dictionary =
      new excess::concurrent_weak_dictionary_HSHTBH<int,void>
            (MAX_DICTIONARY_SIZE, MAX_CPUS);
#else
    std::cerr << "Error: Compiled without HSHT support!" << std::endl;
    exit(-1);
#endif
    break;
  case 6:
#ifdef USE_HSHT
    weak_dictionary =
      new excess::concurrent_weak_dictionary_HSHTC<int,void>
            (MAX_DICTIONARY_SIZE, MAX_CPUS);
#else
    std::cerr << "Error: Compiled without HSHT support!" << std::endl;
    exit(-1);
#endif
    break;
  case 7:
#ifdef USE_HSHT
    weak_dictionary =
      new excess::concurrent_weak_dictionary_HSHTH<int,void>
            (MAX_DICTIONARY_SIZE, MAX_CPUS);
#else
    std::cerr << "Error: Compiled without HSHT support!" << std::endl;
    exit(-1);
#endif
    break;
  }
}

void NBLExpWeakDictionaryBase::WeakDictionaryInsert
  (weak_dictionary_t::handle* handle,
   int key, void *value,
   long& count)
{
  handle->insert(key, value);
  count++;
}

bool NBLExpWeakDictionaryBase::WeakDictionaryLookup
  (weak_dictionary_t::handle* handle,
   int key, void*& value,
   long& countOk, long& countNotFound)
{
  if (handle->lookup(key, value)) {
    countOk++;
    return true;
  } else {
    countNotFound++;
    return false;
  }
}

void NBLExpWeakDictionaryBase::WeakDictionaryRemove
  (weak_dictionary_t::handle* handle,
   int key, void*& value,
   long& count)
{
  handle->remove(key);
  count++;
}

void NBLExpWeakDictionaryBase::WeakDictionaryFreeHandle
  (weak_dictionary_t::handle* handle)
{
  delete handle;
}

NBLExpWeakDictionaryBase::weak_dictionary_t::handle*
NBLExpWeakDictionaryBase::ThreadInitImplementationNr(int nr)
{
  return weak_dictionary->get_handle();
}

void NBLExpWeakDictionaryBase::DeInitImplementationNr(int nr)
{
  delete weak_dictionary;
}

string NBLExpWeakDictionaryBase::GetStatistics()
{
  std::stringstream ss;
  ss << countInsert << "  "
     << countOkLookup << " "
     << countNotFoundLookup << " "
     << countRemove;
  return ss.str();
}

string NBLExpWeakDictionaryBase::GetStatisticsLegend()
{
  std::stringstream ss;
  ss << "<#Insert operations>"
     << " <#Ok Lookup operations>"
     << " <#NotFound Lookup operations>"
     << " <#Remove operations>";
  return ss.str();
}

void NBLExpWeakDictionaryBase::SaveThreadStatistics(long countInsert,
                                                    long countOkLookup,
                                                    long countNotFoundLookup,
                                                    long countRemove)
{
  // Update the global operation counters.
  STDFAA(&this->countInsert, countInsert);
  STDFAA(&this->countOkLookup, countOkLookup);
  STDFAA(&this->countNotFoundLookup, countNotFoundLookup);
  STDFAA(&this->countRemove, countRemove);
}
