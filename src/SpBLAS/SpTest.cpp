// Minimal sparse double matrix GEMM test.
//   Copyright (C) 2015  Anders Gidenstam

#include <ctime>
#include <iostream>
#include <sstream>

#include <EXCESS/concurrent_bag>

#ifdef USE_LIBRSB 
#include <rsb.h>
#endif

#include "SpMatrix.h"
#include "SpGEMM_Gustavson.h"
#include "SpGEMM_DSParallel.h"

static void test_SpMM(char* filename);
#ifdef USE_LIBRSB
static void test_librsb(char* filename);
static std::string rsb_errortostr(rsb_err_t);
#endif

int main(int argc, char** argv)
{
  if (argc < 2) return 0;

#ifdef USE_LIBRSB
  std::cout << std::endl << "Testing librsb SpGEMM." << std::endl;
  test_librsb(argv[1]);
#endif

  std::cout << std::endl << "Testing own SpGEMM." << std::endl;
  test_SpMM(argv[1]);

  return 0;
}

static void test_SpMM(char* filename)
{
  struct timespec t1, t2;

  std::cout << "Attempting to load the matrix '" << filename << "' into A ... ";
  clock_gettime(CLOCK_REALTIME, &t1);
  SpMatrix A = SpMatrix::LoadFromFile(filename);
  clock_gettime(CLOCK_REALTIME, &t2);

  std::cout << "Ok." << std::endl;
  std::cout << "Got " << (A.m) << "x" << (A.n) << " matrix with "
            << (A.nzmax) << " non-zeros." << std::endl;
  std::cout << "Duration: "
            << ((double)(t2.tv_sec - t1.tv_sec) +
                1e-9 * (double)(t2.tv_nsec - t1.tv_nsec))
            << " sec" << std::endl;

  {
    std::cout << "Attempting matrix matrix multiplication "
              << "C = SpMM_Gustavson(A, A) ... ";

    clock_gettime(CLOCK_REALTIME, &t1);
    SpMatrix C = SpMM_Gustavson_RowStore(A, A);
    clock_gettime(CLOCK_REALTIME, &t2);

    std::cout << "Ok." << std::endl;
    std::cout << "Result C is " << (C.m) << "x" << (C.n) << " matrix with "
            << (C.nzmax) << " non-zeros." << std::endl;
    std::cout << "Duration: "
              << ((double)(t2.tv_sec - t1.tv_sec) +
                  1e-9 * (double)(t2.tv_nsec - t1.tv_nsec))
              << " sec" << std::endl;
  }

  {
    std::cout << "Attempting matrix matrix multiplication "
              << "C = SpMM_DSParallel(A, A) ... ";

    clock_gettime(CLOCK_REALTIME, &t1);
    SpMatrix C =
      SpMM_DSParallel_RowStore<excess::concurrent_bag_TBBQueue>(A, A);
      //SpMM_DSParallel_RowStore<excess::concurrent_bag_NBLBag>(A, A);
    clock_gettime(CLOCK_REALTIME, &t2);

    std::cout << "Ok." << std::endl;
    std::cout << "Result C is " << (C.m) << "x" << (C.n) << " matrix with "
              << (C.nzmax) << " non-zeros." << std::endl;
    std::cout << "Duration: "
              << ((double)(t2.tv_sec - t1.tv_sec) +
                  1e-9 * (double)(t2.tv_nsec - t1.tv_nsec))
              << " sec" << std::endl;
  }
}

#ifdef USE_LIBRSB
static void test_librsb(char* filename)
{
  struct timespec t1, t2;
  int errval;

  if (RSB_ERR_NO_ERROR != (errval = rsb_lib_init(RSB_NULL_INIT_OPTIONS))) {
    std::cerr << "Error initializing librsb! '"
              << rsb_errortostr(errval) << "'"
              << std::endl;
    return;
  }
  std::cout << "Correctly initialized librsb." << std::endl;

  std::cout << "Attempting to load the matrix '" << filename << "' into A ... ";
  clock_gettime(CLOCK_REALTIME, &t1);
  struct rsb_mtx_t* A =
    rsb_file_mtx_load(filename,
                      RSB_FLAG_NOFLAGS,
                      RSB_NUMERICAL_TYPE_DOUBLE,
                      &errval);
  clock_gettime(CLOCK_REALTIME, &t2);
  if (!A) {
    std::cerr << "Error loading A! '"
              << rsb_errortostr(errval) << "'"
              << std::endl;
    return;
  } else {
    std::cout << "Ok." << std::endl;
    int m, n, nnz;
    rsb_mtx_get_info(A, RSB_MIF_MATRIX_ROWS__TO__RSB_COO_INDEX_T, &m);
    rsb_mtx_get_info(A, RSB_MIF_MATRIX_COLS__TO__RSB_COO_INDEX_T, &n);
    rsb_mtx_get_info(A, RSB_MIF_MATRIX_NNZ__TO__RSB_NNZ_INDEX_T, &nnz);
    std::cout << "Got " << m << "x" << n << " matrix with "
              << nnz << " non-zeros." << std::endl;
    std::cout << "Duration: "
              << ((double)(t2.tv_sec - t1.tv_sec) +
                  1e-9 * (double)(t2.tv_nsec - t1.tv_nsec))
              << " sec" << std::endl;
  }

  {
    std::cout << "Attempting matrix matrix multiplication "
              << "C = rsb_spmsp(A, A) ... ";

    clock_gettime(CLOCK_REALTIME, &t1);
    struct rsb_mtx_t* C =
      rsb_spmsp(RSB_NUMERICAL_TYPE_DOUBLE,
                RSB_TRANSPOSITION_N,
                NULL,
                A,
                RSB_TRANSPOSITION_N,
                NULL,
                A,
                &errval);
    clock_gettime(CLOCK_REALTIME, &t2);
    if (!C) {
      std::cerr << "Error! '"
                << rsb_errortostr(errval) << "'"
                << std::endl;
    } else {
      std::cout << "Ok." << std::endl;
      int m, n, nnz;
      rsb_mtx_get_info(C, RSB_MIF_MATRIX_ROWS__TO__RSB_COO_INDEX_T, &m);
      rsb_mtx_get_info(C, RSB_MIF_MATRIX_COLS__TO__RSB_COO_INDEX_T, &n);
      rsb_mtx_get_info(C, RSB_MIF_MATRIX_NNZ__TO__RSB_NNZ_INDEX_T, &nnz);

      std::cout << "Result C is " << m << "x" << n << " matrix with "
                << nnz << " non-zeros." << std::endl;
      std::cout << "Duration: "
                << ((double)(t2.tv_sec - t1.tv_sec) +
                    1e-9 * (double)(t2.tv_nsec - t1.tv_nsec))
                << " sec" << std::endl;
    }

    if (C) {
      rsb_mtx_free(C);
    }
  }

  if (A) {
    rsb_mtx_free(A);
  }
  if (RSB_ERR_NO_ERROR != (errval = rsb_lib_exit(RSB_NULL_EXIT_OPTIONS))) {
    std::cerr << "Error closing librsb! '"
              << rsb_errortostr(errval) << "'"
              << std::endl;
  }
}
#endif

#ifdef USE_LIBRSB
static std::string rsb_errortostr(rsb_err_t errval)
{
  rsb_char_t buf[500];
  if (RSB_ERR_NO_ERROR == rsb_strerror_r (errval,
                                          buf,
                                          500)) {
    return std::string(buf);
  } else {
    return "Failed to get error string.";
  }
}
#endif
