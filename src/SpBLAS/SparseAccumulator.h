// Sparse accumulator for double matrix GEMM implementations
//   Copyright (C) 2015  Anders Gidenstam

#ifndef __SPARSE_ACCUMULATOR_H
#define __SPARSE_ACCUMULATOR_H

#include <vector>

#include <cassert>
#include <cstdlib>
#include <iostream>

class SparseAccumulator // a.k.a. SPA.
{
public: // For now.
  int              n;
  int              elems;
  int              version;
  double*          v;
  int*             used;
  std::vector<int> nzs;

  static const double zero;

public:
  SparseAccumulator(int n);
  ~SparseAccumulator();

  void Clear();
  void AddTo(int index, double value);

  const double& operator[](int index);

};

#endif
