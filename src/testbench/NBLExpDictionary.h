// Dictionary microbenchmark for the experiment framework.
//   Copyright (C) 2015  Anders Gidenstam
//
#pragma once
#include "NBLExpDictionaryBase.h"

#include <map>

class NBLExpDictionary :
  public NBLExpDictionaryBase
{
public:
  NBLExpDictionary(void);
  virtual ~NBLExpDictionary(void);
  virtual string GetExperimentName();
  virtual string GetCaseName();
  virtual vector<string> GetParameters();
  virtual vector<string> GetParameterValues(int pno);
  virtual int  GetParameter(int pno);
  virtual void SetParameter(int pno, int value);
  virtual void CreateScenario();
  virtual void InitImplementationNr(int nr); // Prepare for a case.
  virtual void RunImplementationNr(int nr, int threadID); // For each thread
  virtual string GetStatistics();
  virtual string GetStatisticsLegend();

protected:
  enum Op_t { UPDATE, LOOKUP };
  typedef std::pair<Op_t,int> OpDesc_t;
  static const int MAX_OPS = 1048576;
  // Experiment setup.
  int KEY_SPACE;
  int INITIAL_SIZE;
  int UPDATE_PERCENTAGE;
  int PARALLEL_WORK;
  // Experiment control and result collection.
  std::map<int,void*> *initialState;
  OpDesc_t opSchedule[MAX_CPUS][MAX_OPS];
  volatile int active;
};
