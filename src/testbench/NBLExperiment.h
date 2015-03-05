// Noble/EXCESS experiment framework.
//   Copyright (C) 2011  Håkan Sundell
//   Copyright (C) 2014 - 2015  Anders Gidenstam (adapted for the EXCESS
//                                                framework)
#pragma once

#include <string>
using std::string;

#include <vector>
using std::vector;

class NBLExperiment
{
public:
  int resultType;
  string resultString;
public:
  NBLExperiment(void);
  virtual ~NBLExperiment(void);
  virtual string GetExperimentName() = 0;
  virtual string GetCaseName() = 0;
  virtual void SetNrThreads(int nrOfThreads) = 0;
  virtual vector<string> GetImplementations() = 0;

  virtual vector<string> GetParameters() = 0;
  virtual vector<string> GetParameterValues(int pno) = 0;
  virtual int  GetParameter(int pno) = 0;
  virtual void SetParameter(int pno, int value) = 0;

  virtual void CreateScenario() = 0;// Random scenario
  virtual void InitImplementationNr(int nr) = 0;
  virtual void DeInitImplementationNr(int nr) = 0;
  virtual void RunImplementationNr(int nr, int threadID) = 0; // For each thread
  virtual string GetStatistics() = 0;
  virtual string GetStatisticsLegend() = 0;
};
