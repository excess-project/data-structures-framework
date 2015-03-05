// Producer-Consumer microbenchmark for the experiment framework.
//   Copyright (C) 2014 - 2015  Anders Gidenstam
// Based on NBLExpQueue.h
//   Copyright (C) 2011  Håkan Sundell
//
#pragma once
#include "NBLExpProducerConsumerBase.h"

class NBLExpProducerConsumer :
  public NBLExpProducerConsumerBase
{
public:
  NBLExpProducerConsumer(void);
  virtual ~NBLExpProducerConsumer(void);
  virtual string GetExperimentName();
  virtual string GetCaseName();
  virtual vector<string> GetParameters();
  virtual vector<string> GetParameterValues(int pno);
  virtual int  GetParameter(int pno);
  virtual void SetParameter(int pno, int value);
  virtual void CreateScenario(); // Random scenario
  virtual void RunImplementationNr(int nr, int threadID); // For each thread
  virtual string GetStatistics();
  virtual string GetStatisticsLegend();

protected:
  // Experiment setup.
  int COMM_NR;
  int PRODUCER_PW;
  int CONSUMER_PW;
  int MAX_INSERTS;
  // Experiment control and result collection.
  volatile int active;
  int          no_phases;
  long double* phases;
  long double  active_duration;
};
