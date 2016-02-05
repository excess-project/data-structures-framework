// Sparse matrix-matrix multiplication benchmark for the EXCESS experiment
// framework.
//   Copyright (C) 2015 - 2016  Anders Gidenstam
//
#pragma once
#include "NBLExpProducerConsumerBase.h"

#include <SpMatrix.h>

class NBLExpSpGEMM : public NBLExpProducerConsumerBase
{
public:
  NBLExpSpGEMM(void);
  ~NBLExpSpGEMM(void);
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
  void SpMM_DSParallel_RowStore_1P(const SpMatrix& A, const SpMatrix& B,
                                   handle_t* Ci_bag,
                                   volatile int* nextci, int* element_count,
                                   long& countInsert);
  void SpMM_DSParallel_RowStore_2S(SpMatrix& C, int* element_count);
  void SpMM_DSParallel_RowStore_3P(SpMatrix& C,
                                   handle_t* Ci_bag,
                                   int* element_count,
                                   long& countOkTryRemove,
                                   long& countEmptyTryRemove);

  // Experiment setup.
  //   Work unit size in #rows.
  int WUSize;
  // Experiment control and result collection.
  int matrix;
  int mmalg;
  SpMatrix A;
  SpMatrix C;
  volatile int phase1;
  volatile int phase2;
  long double  phase2_start;
  volatile int phase3;
  long double  phase3_start;
  volatile int nextci;
  int* element_count;
};
