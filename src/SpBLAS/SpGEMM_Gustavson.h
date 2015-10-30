// Sparse double matrix GEMM implementation based on the classical algorithm
// in [Gustavson, ACM TMS 4(3), 1978].
//   Copyright (C) 2015  Anders Gidenstam

#ifndef __SPGEMM_GUSTAVSON_H
#define __SPGEMM_GUSTAVSON_H

#include <vector>

#include <cassert>
#include <iostream>

#include "SpMatrix.h"
#include "SparseAccumulator.h"

SpMatrix SpGEMM_Gustavson_RowStore(double alpha, const SpMatrix& A,
                                   double beta,  const SpMatrix& B)
{
  assert(A.n == B.m);
  std::vector< SpMatrix::MatrixRow_t* > Ci;
  int* element_count = (int*)calloc(A.m, sizeof(int));

  {
    double alphabeta = alpha*beta;
    SparseAccumulator SPA(B.n);

    for (int ci = 0; ci < A.m; ++ci) {
      int aij  = A.rp[ci];
      int aend =  A.rp[ci + 1];
      for (; aij < aend; ++aij) {
        int j    = A.ci[aij];
        int bjk  = B.rp[j];
        int bend = B.rp[j+1];
        int vaij = A.v[aij];
        for (; bjk < bend; ++bjk) {
          int k = B.ci[bjk];
          SPA.AddTo(k, vaij*B.v[bjk]);
        }
      }
      // Save Ci* that is stored in the SPA.
      std::vector<int>::const_iterator cik = SPA.nzs.begin();
      std::vector<int>::const_iterator cend  = SPA.nzs.end();

      int nnz = 0;
      SpMatrix::MatrixRow_t* row = new SpMatrix::MatrixRow_t(ci, cend - cik);
      for (; cik < cend; ++cik) {
        double vcik = alphabeta*SPA.v[*cik];
        if (vcik != 0.0) { // FIXME: cut off very small values too?
          row->ci[nnz] = *cik;
          row->v[nnz]  = vcik;
          nnz++;
        }
      }
      // Save the true #nz.
      element_count[ci] = nnz;
      Ci.push_back(row);
      SPA.Clear();
    }
  }

  // Create the result matrix C.
  SpMatrix C(A.m, B.n, 0);

  {
    // Prepare the row pointers.
    int nnz = 0;
    int ri;
    for (ri = 0; ri < C.m; ++ri) {
      C.rp[ri] = nnz;
      nnz += element_count[ri];
    }
    C.rp[ri] = nnz;

    // Reallocate the ci and v vectors in C.
    C.nzmax = nnz;
    std::free(C.ci);
    std::free(C.v);
    C.ci = (int*)std::calloc(C.nzmax, sizeof(int));
    C.v  = (double*)std::calloc(C.nzmax, sizeof(double));
  }

  // Fill the result matrix C.
  while (!Ci.empty()) {
    SpMatrix::MatrixRow_t* ci = Ci.back();
    Ci.pop_back();

    int kbegin = C.rp[ci->row];
    int knnz   = element_count[ci->row];
#ifdef _MEMCPY
    // It is not clear if memcpy is any faster. The bigger loop body below
    // might allow better optimization.
    std::memcpy(&C.ci[kbegin], ci->ci, knnz * sizeof(int));
    std::memcpy(&C.v[kbegin], ci->v, knnz * sizeof(double));
#else
    for (int k = 0; k < knnz; ++k) {
      C.ci[kbegin + k] = ci->ci[k];
      C.v[kbegin + k]  = ci->v[k];
    }
#endif
    delete ci;
  }

  std::free(element_count);
  return C;
}

SpMatrix SpMM_Gustavson_RowStore(const SpMatrix& A, const SpMatrix& B)
{
  return SpGEMM_Gustavson_RowStore(1.0, A, 1.0, B);
}

SpMatrix SpGEMM_Gustavson_TripletStore(double alpha, const SpMatrix& A,
                                       double beta,  const SpMatrix& B)
{
  assert(A.n == B.m);
  double alphabeta = alpha*beta;
  SparseAccumulator SPA(B.n);
  std::vector< SpMatrix::MatrixTriple_t > Cik;

  for (int ci = 0; ci < A.m; ci++) {
    int aij  = A.rp[ci];
    int aend =  A.rp[ci + 1];
    for (; aij < aend; ++aij) {
      int j    = A.ci[aij];
      int bjk  = B.rp[j];
      int bend = B.rp[j+1];
      int vaij = A.v[aij];
      for (; bjk < bend; ++bjk) {
        int k = B.ci[bjk];
        SPA.AddTo(k, vaij*B.v[bjk]);
      }
    }
    // Save Ci* that is stored in the SPA.
    std::vector<int>::const_iterator cik = SPA.nzs.begin();
    std::vector<int>::const_iterator cend  = SPA.nzs.end();
    for (; cik < cend; ++cik) {
      double vcik = alphabeta*SPA.v[*cik];
      if (vcik != 0.0) { // FIXME: cut off very small values too?
        Cik.push_back
          (SpMatrix::MatrixTriple_t(SpMatrix::RowColumnPair_t(ci,*cik),
                                    vcik));
      }
    }
    SPA.Clear();
  }
  // Create the result matrix C.
  return SpMatrix(A.m, B.n, Cik);
}

SpMatrix SpMM_Gustavson_TripletStore(const SpMatrix& A, const SpMatrix& B)
{
  return SpGEMM_Gustavson_TripletStore(1.0, A, 1.0, B);
}

#endif
