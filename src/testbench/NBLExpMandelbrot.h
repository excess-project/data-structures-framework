// Mandelbrot application benchmark for the experiment framework.
//   Copyright (C) 2011  Håkan Sundell
//                 2014  Anders Gidenstam (Adaptation to the CLI frontend and
//                                         ProducerConsumer base class.)
//

#pragma once
#include "NBLExpProducerConsumerBase.h"

class NBLExpApplicationMandelbrot: public NBLExpProducerConsumerBase
{
public:
  NBLExpApplicationMandelbrot(void);
  virtual ~NBLExpApplicationMandelbrot(void);
  virtual string GetExperimentName();
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
  int PATTERN_NR;
  int CONTENTION_NR;
};

