// Concurrent data structure based parallel sparse double matrix GEMM
// implementation.
// New(?) SpMM_DSParallel_RowStore/TripletStore algorithms by Anders Gidenstam,
// inspired by the classical sequential algorithm in [Gustavson, ACM TMS 4(3),
// 1978].
//   Copyright (C) 2015  Anders Gidenstam

#ifndef __SPGEMM_CDS_H
#define __SPGEMM_CDS_H

#if USE_OPENMP
#include <omp.h>
#endif

#include <algorithm>
#include <vector>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "primitives.h"
#include "SpMatrix.h"
#include "SparseAccumulator.h"

#define WU 4

/*
// Split into its phases below.
template < template < typename T > class concurrent_bag_t >
SpMatrix SpMM_DSParallel_RowStore(const SpMatrix& A, const SpMatrix& B)
{
  assert(A.n == B.m);
  volatile int nextci = 0;
  concurrent_bag_t< SpMatrix::MatrixRow_t >* Ci_bag =
    new concurrent_bag_t< SpMatrix::MatrixRow_t >();
  int* element_count = (int*)calloc(A.m, sizeof(int));

#pragma omp parallel
  {
    typename concurrent_bag_t< SpMatrix::MatrixRow_t >::handle* Ci =
      Ci_bag->get_handle();
    SparseAccumulator SPA(B.n);
    int ci;

    while (1) {

      ci = FAA32(&nextci, WU);
      int ciend = std::min(ci+WU, A.m);

      if (!(ci < A.m)) {
        break;
      }

      for (; ci < ciend; ++ci) {
        int aij  = A.rp[ci];
        int aend = A.rp[ci + 1];
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
          double vcik = SPA.v[*cik];
          if (vcik != 0.0) { // FIXME: cut off very small values too?
            row->ci[nnz] = *cik;
            row->v[nnz]  = vcik;
            nnz++;
          }
        }
        // Save the true #nz.
        element_count[ci] = nnz;
        Ci->insert(row);
        SPA.Clear();
      }
    }
    delete Ci;
  } // end parallel

  // Create the result matrix C.
  SpMatrix C(A.m, B.n, 0);

  // Prepare the row pointers.
  int nnz = 0;
  int ci;
  for (ci = 0; ci < C.m; ++ci) {
    C.rp[ci] = nnz;
    nnz += element_count[ci];
  }
  C.rp[ci] = nnz;

  // Reallocate the ci and v vectors in C.
  C.nzmax = nnz;
  std::free(C.ci);
  std::free(C.v);
  C.ci = (int*)std::calloc(C.nzmax, sizeof(int));
  C.v  = (double*)std::calloc(C.nzmax, sizeof(double));

  // Fill C in parallel from the triplet store.
#pragma omp parallel
  {
    typename concurrent_bag_t< SpMatrix::MatrixRow_t >::handle* Ci =
      Ci_bag->get_handle();
    SpMatrix::MatrixRow_t* ci;
    while (Ci->try_remove_any(ci)) {
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
    delete Ci;
  } // end parallel

  return C;
}
*/

// Internal function: 1st parallel pass.
template < template < typename T > class concurrent_bag_t >
void SpMM_DSParallel_RowStore_1P
  (const SpMatrix& A, const SpMatrix& B,
   concurrent_bag_t< SpMatrix::MatrixRow_t >* Ci_bag,
   volatile int* nextci,
   int* element_count)
{
  typename concurrent_bag_t< SpMatrix::MatrixRow_t >::handle* Ci =
    Ci_bag->get_handle();
  SparseAccumulator SPA(B.n);
  int ci;

  while (1) {

    ci = FAA32(nextci, WU);
    int ciend = std::min(ci+WU, A.m);

    if (!(ci < A.m)) {
      break;
    }

    for (; ci < ciend; ++ci) {
      int aij  = A.rp[ci];
      int aend = A.rp[ci + 1];
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
        double vcik = SPA.v[*cik];
        if (vcik != 0.0) { // FIXME: cut off very small values too?
          row->ci[nnz] = *cik;
          row->v[nnz]  = vcik;
          nnz++;
        }
      }
      // Save the true #nz.
      element_count[ci] = nnz;
      Ci->insert(row);
      SPA.Clear();
    }
  }
  delete Ci;
}

// Internal function: Sequential pass.
template < template < typename T > class concurrent_bag_t >
void SpMM_DSParallel_RowStore_2S
  (SpMatrix& C,
   int* element_count)
{
  // Prepare the row pointers.
  int nnz = 0;
  int ci;
  for (ci = 0; ci < C.m; ++ci) {
    C.rp[ci] = nnz;
    nnz += element_count[ci];
  }
  C.rp[ci] = nnz;

  // Reallocate the ci and v vectors in C.
  C.nzmax = nnz;
  std::free(C.ci);
  std::free(C.v);
  C.ci = (int*)std::calloc(C.nzmax, sizeof(int));
  C.v  = (double*)std::calloc(C.nzmax, sizeof(double));
}

// Internal function: 2nd parallel pass.
template < template < typename T > class concurrent_bag_t >
void SpMM_DSParallel_RowStore_3P
  (SpMatrix& C,
   concurrent_bag_t< SpMatrix::MatrixRow_t >* Ci_bag,
   int* element_count)
{
  typename concurrent_bag_t< SpMatrix::MatrixRow_t >::handle* Ci =
    Ci_bag->get_handle();
  SpMatrix::MatrixRow_t* ci;
  while (Ci->try_remove_any(ci)) {
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
  delete Ci;
}

template < template < typename T > class concurrent_bag_t >
SpMatrix SpMM_DSParallel_RowStore(const SpMatrix& A, const SpMatrix& B)
{
  assert(A.n == B.m);
  volatile int nextci = 0;
  concurrent_bag_t< SpMatrix::MatrixRow_t >* Ci_bag =
    new concurrent_bag_t< SpMatrix::MatrixRow_t >();
  int* element_count = (int*)calloc(A.m, sizeof(int));

#pragma omp parallel
  { 
    SpMM_DSParallel_RowStore_1P<concurrent_bag_t>(A, B,
                                                  Ci_bag,
                                                  &nextci, element_count);
  }

  // Create the result matrix C.
  SpMatrix C(A.m, B.n, 0);

  // Sequential preparation of C.
  SpMM_DSParallel_RowStore_2S<concurrent_bag_t>(C, element_count);

  // Fill C in parallel from the row store.
#pragma omp parallel
  { 
    SpMM_DSParallel_RowStore_3P<concurrent_bag_t>(C, Ci_bag, element_count);
  }

  return C;
}


template < template < typename T > class concurrent_bag_t >
SpMatrix SpMM_DSParallel_TripletStore(const SpMatrix& A, const SpMatrix& B)
{
  assert(A.n == B.m);
  volatile int nextci = 0;
  concurrent_bag_t< SpMatrix::MatrixTriple_t >* Cik_bag =
    new concurrent_bag_t< SpMatrix::MatrixTriple_t >();
  int* element_count = (int*)calloc(A.m, sizeof(int));

#pragma omp parallel
  {
    typename concurrent_bag_t< SpMatrix::MatrixTriple_t >::handle* Cik =
      Cik_bag->get_handle();
    SparseAccumulator SPA(B.n);
    int ci;

    while (1) {

      ci = FAA32(&nextci, WU);
      int ciend = std::min(ci+WU, A.m);

      if (!(ci < A.m)) {
        break;
      }

      for (; ci < ciend; ++ci) {
        int aij  = A.rp[ci];
        int aend = A.rp[ci + 1];
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
        for (; cik < cend; ++cik) {
          double vcik = SPA.v[*cik];
          if (vcik != 0.0) { // FIXME: cut off very small values too?
            nnz++;
            Cik->insert
              (new SpMatrix::MatrixTriple_t(SpMatrix::RowColumnPair_t(ci,*cik),
                                            vcik));
          }
        }
        element_count[ci] = nnz;
        SPA.Clear();
      }
    }
    delete Cik;
  } // end parallel

  // Create the result matrix C.
  SpMatrix C(A.m, B.n, 0);

  // Prepare the row pointers.
  int nnz = 0;
  int ci;
  for (ci = 0; ci < C.m; ++ci) {
    nnz += element_count[ci];
    C.rp[ci] = nnz;
  }
  C.rp[ci] = nnz;

  // Reallocate the ci and v vectors in C.
  C.nzmax = nnz;
  std::free(C.ci);
  std::free(C.v);
  C.ci = (int*)std::calloc(C.nzmax, sizeof(int));
  C.v  = (double*)std::calloc(C.nzmax, sizeof(double));

  // Fill C in parallel from the triplet store.
#pragma omp parallel
  {
    typename concurrent_bag_t< SpMatrix::MatrixTriple_t >::handle* Cik =
      Cik_bag->get_handle();
    SpMatrix::MatrixTriple_t* cik;
    while (Cik->try_remove_any(cik)) {
      int i    = cik->first.first;
      int k    = cik->first.second;
      double v = cik->second;
      delete cik;
      int idx = FAA32(&C.rp[i],-1) - 1;
      if (idx < 0) {
        std::cout << "Bad idx for triple (" << i << ", " << k << ", "
                  << v << ")" << std::endl;
      }
      C.ci[idx] = k;
      C.v[idx]  = v;
    }
    delete Cik;
  } // end parallel

  //std::cout << "SpMM(): Got " << Cik.size() << " non-zeros.";
  return C;
}

#endif
