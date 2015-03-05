// Matrix-matrix multiplication benchmark for the EXCESS experiment framework.
//   Copyright (C) 2014 - 2015  Anders Gidenstam
//
#pragma once
#include "NBLExpProducerConsumerBase.h"

class NBLExpGEMM : public NBLExpProducerConsumerBase
{
public:
  NBLExpGEMM(void);
  ~NBLExpGEMM(void);
  virtual string GetExperimentName();
  virtual string GetCaseName();
  virtual vector<string> GetParameters();
  virtual vector<string> GetParameterValues(int pno);
  virtual int  GetParameter(int pno);
  virtual void SetParameter(int pno, int value);
  virtual void CreateScenario(); // Random scenario
  virtual void InitImplementationNr(int nr);
  virtual void RunImplementationNr(int nr, int threadID); // For each thread
  virtual string GetStatistics();
  virtual string GetStatisticsLegend();

protected:
  // Experiment setup.
  //   Matrix dimension (square matrices)
  int N;
  //   Sub-matrix (work unit) size. Should be power of 2.
  int Nwu;
  // Experiment control and result collection.
  float *A, *B, *C, *Src;
  volatile int added;
};
