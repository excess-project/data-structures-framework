// Sparse matrix-matrix multiplication benchmark for the EXCESS experiment
// framework.
//   Copyright (C) 2015 - 2016  Anders Gidenstam
//

#include "NBLExpSpGEMM.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <ctime>
#include <unistd.h>

#include "primitives.h"
#include "SparseAccumulator.h"
#include "SpGEMM_Gustavson.h"

static bool VerifySpMMSquare(const SpMatrix A, const SpMatrix C);

NBLExpSpGEMM::NBLExpSpGEMM(void)
  : WUSize(4), matrix(0), mmalg(1),
    A(SpMatrix(0, 0, 0)), C(SpMatrix(0, 0, 0)),
    phase1(0), phase2(0), phase3(0), nextci(0), element_count(0)
{
}

NBLExpSpGEMM::~NBLExpSpGEMM(void)
{
  /*
  std::cout << "Verifying correctness of the answer  ... ";
  if (VerifyGEMM(A, B, C, Src, N)) {
    std::cout << "OK!";
  } else {
    std::cout << "FAILED!";
  }
  std::cout << std::endl;
  */

  std::free(element_count);
}

string NBLExpSpGEMM::GetExperimentName()
{
  return string("SpGEMM");
}

string NBLExpSpGEMM::GetCaseName()
{
  std::stringstream ss;
  ss << "-M" << matrix
     << "-MM" << mmalg
     << "-WU" << WUSize;
  return ss.str();
}

vector<string> NBLExpSpGEMM::GetParameters()
{
  vector<string> v;
  v.push_back(string("Matrix"));
  v.push_back(string("Algorithm"));
  v.push_back(string("Work unit size (rows)"));
  return v;
}


vector<string> NBLExpSpGEMM::GetParameterValues(int pno)
{
  vector<string> v;
  switch (pno) {
  case 0:
    v.push_back(string("Matrix 0. R14; 1. R15; 2. R16 (default 0)"));
    break;
  case 1:
    v.push_back(string("Matrix-matrix multiplication algorithm 0. sequential Gustavson. 1. new D-S parallel (default 1)"));
    break;
  case 2:
    v.push_back(string("Work unit size in rows of A (default 4)"));
    break;
  }
  return v;
}

int NBLExpSpGEMM::GetParameter(int pno)
{
  switch (pno) {
  case 0:
    return matrix;
  case 1:
    return mmalg;
  case 2:
    return WUSize;
  default:
    return -1;
  }
}

void NBLExpSpGEMM::SetParameter(int pno, int value)
{
  switch (pno) {
  case 0:
    matrix=value;
    break;
  case 1:
    mmalg=value;
  case 2:
    WUSize=value;
  default:
    break;
  }
}

void NBLExpSpGEMM::CreateScenario()
{
  int i;

  // Load the matrix A.
  switch (matrix) {
  case 0:
    A = SpMatrix::LoadFromFile("R14.mm");
    break;
  case 1:
    A = SpMatrix::LoadFromFile("R15.mm");
    break;
  case 2:
    A = SpMatrix::LoadFromFile("R16.mm");
    break;
  default:
    A = SpMatrix::LoadFromFile("R14.mm");
  }

  // Create the result matrix C.
  C = SpMatrix(A.m, A.n, 0);
  element_count = (int*)calloc(A.m, sizeof(int));

  //std::cout << "Created the matrix A as " << (A.m) << "x" << (A.n)
  //          << " matrix with " << (A.nzmax) << " non-zeros." << std::endl;
  //std::cout << "Created the matrix C as " << (C.m) << "x" << (C.n)
  //          << " matrix with " << (C.nzmax) << " non-zeros." << std::endl;
}

void NBLExpSpGEMM::InitImplementationNr(int nr)
{
  NBLExpProducerConsumerBase::InitImplementationNr(nr);
  // Initialization for one m-m multiplication.
  phase1 = 0;
  phase2 = 0;
  phase2_start = 0;
  phase3 = 0;
  phase3_start = 0;
  nextci = 0;
  for (int i = 0; i < A.m; ++i) {
    element_count[i] = 0;
  }
  //std::cout << "Preparing to square a " << (A.m) << "x" << (A.n)
  //          << " matrix with " << (A.nzmax) << " non-zeros." << std::endl;
}

void NBLExpSpGEMM::RunImplementationNr(int nr, int threadID)
{
  long countInsert = 0;
  long countOkTryRemove = 0;
  long countEmptyTryRemove = 0;
  struct timespec cur_time;

  handle_t* handle = ThreadInitImplementationNr(nr);

  int myId=(int)threadID;

  switch (mmalg) {
  case 0:
    if (!FAA32(&phase1, 1)) {
      // Sequential in one thread.
      C = SpMM_Gustavson_RowStore(A, A);
    }
    break;

  case 1:
    // New(?) SpMM_DSParallel_RowStore algorithm by Anders Gidenstam, inspired
    // by the classical sequential algorithm in [Gustavson, ACM TMS 4(3), 1978].

    // The SpMM_DSParallel_RowStore algorithm goes in 3 phases.

    // Parallel phase 1.
    {
      SpMM_DSParallel_RowStore_1P(A, A, handle, &nextci, element_count,
                                  countInsert);
      FAA32(&phase1, 1);
      // Wait for all.
      while (phase1 < NR_CPUS) {
        usleep(0);
      }
    }
    
    // Sequential phase 2.
    {
      int p2 = phase2;
      if (!p2) {
        p2 = FAA32(&phase2, 1);
      }
      if (!p2) {
        // Perform the sequential phase 2.
        clock_gettime(CLOCK_REALTIME, &cur_time);
        phase2_start = ((long double)cur_time.tv_sec +
                        1e-9 * (long double)cur_time.tv_nsec);

        SpMM_DSParallel_RowStore_2S(C, element_count);
        
        // Launch phase 3.
        FAA32(&phase3, 1);
        clock_gettime(CLOCK_REALTIME, &cur_time);
        phase3_start = ((long double)cur_time.tv_sec +
                        1e-9 * (long double)cur_time.tv_nsec);
      } else {
        // Wait for phase 3 to begin.
        while (phase3 == 0) {
          usleep(0);
        }
      }
    } // end phase 2
    
    // Parallel phase 3.
    SpMM_DSParallel_RowStore_3P(C, handle, element_count,
                                countOkTryRemove, countEmptyTryRemove);
    break;
  }

  // Update the global operation counters from all/active phases.
  SaveThreadStatistics(countInsert,
                       countOkTryRemove, countEmptyTryRemove);
  delete handle;

  //std::cout << "The result is a " << (C.m) << "x" << (C.n)
  //          << " matrix with " << (C.nzmax) << " non-zeros." << std::endl;
}

string NBLExpSpGEMM::GetStatistics()
{
  std::stringstream ss;
  ss << std::setprecision(std::numeric_limits<long double>::digits10)
     << NBLExpProducerConsumerBase::GetStatistics()
     << " " << matrix
     << " " << mmalg
     << " " << A.m
     << " " << WUSize
     << " " << phase2_start
     << " " << phase3_start;
  return ss.str();
}

string NBLExpSpGEMM::GetStatisticsLegend()
{
  std::stringstream ss;
  ss << NBLExpProducerConsumerBase::GetStatisticsLegend()
     << " <matrix no.>"
     << " <mm algorithm>"
     << " <square matrix dimension N>"
     << " <work unit size>"
     << " <phase 2 start time>"
     << " <phase 3 start time>";
  return ss.str();
}

void NBLExpSpGEMM::SpMM_DSParallel_RowStore_1P(const SpMatrix& A,
                                               const SpMatrix& B,
                                               handle_t* Ci_bag,
                                               volatile int* nextci,
                                               int* element_count,
                                               long& countInsert)
{
  SparseAccumulator SPA(B.n);
  int ci;

  while (1) {

    ci = FAA32(nextci, WUSize);
    int ciend = std::min(ci+WUSize, A.m);

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
      Insert(Ci_bag, row, countInsert);
      SPA.Clear();
    }
  }
}

void NBLExpSpGEMM::SpMM_DSParallel_RowStore_2S(SpMatrix& C, int* element_count)
{
  // Prepare the row pointers.
  int nnz = 0;
  int ci;
  for (ci = 0; ci < C.m; ++ci) {
    C.rp[ci] = nnz;
    nnz += element_count[ci];
  }
  C.rp[ci] = nnz;

  //std::cout << "The result has " << nnz << " non-zeros." << std::endl;

  // Reallocate the ci and v vectors in C.
  C.nzmax = nnz;
  std::free(C.ci);
  std::free(C.v);
  C.ci = (int*)std::calloc(C.nzmax, sizeof(int));
  C.v  = (double*)std::calloc(C.nzmax, sizeof(double));
}

void NBLExpSpGEMM::SpMM_DSParallel_RowStore_3P(SpMatrix& C,
                                               handle_t* Ci_bag,
                                               int* element_count,
                                               long& countOkTryRemove,
                                               long& countEmptyTryRemove)
{
  SpMatrix::MatrixRow_t* ci;
  while (ci = (SpMatrix::MatrixRow_t*)TryRemove(Ci_bag,
                                                countOkTryRemove,
                                                countEmptyTryRemove)) {
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
}

static bool VerifySpMMSquare(const SpMatrix A, const SpMatrix C)
{
  return false;
}
