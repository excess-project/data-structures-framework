// Minimal sparse double matrix GEMM test.
//   Copyright (C) 2015 - 2016  Anders Gidenstam

#include <ctime>
#include <iostream>
#include <sstream>

#include <EXCESS/concurrent_bag>

#include "SpMatrix.h"
#include "SpGEMM_Gustavson.h"
static void test_SpMM_sequential(std::string filename);
#include "SpGEMM_DSParallel.h"
static void test_SpMM_DSParallelRS(std::string filename);
static void test_SpMM_DSParallelTS(std::string filename);

#ifdef USE_LIBRSB 
#include <rsb.h>

static void test_librsb(std::string filename);
static std::string rsb_errortostr(rsb_err_t);
#endif

#ifdef USE_COMBBLAS
#include <mpi.h>
#include <CombBLAS.h>

static void test_CombBLAS(std::string filename);
#endif

static void process_arguments(int argc, char** argv);
static void print_usage(int argc, char** argv);
static bool try_parse(std::string s, int& i);

const int no_algorithms = 4;
typedef enum { OWN_RS=0, OWN_TS, GUSTAVSON, LIBRSB, COMBBLAS} algorithm_t;

static algorithm_t algorithm = OWN_RS;
static std::string matrix_filename;

int main(int argc, char** argv)
{
  process_arguments(argc, argv);

  switch (algorithm) {
  case OWN_RS:
    std::cout << std::endl << "Testing own parallel RS SpGEMM." << std::endl;
    test_SpMM_DSParallelRS(matrix_filename);
    break;
  case OWN_TS:
    std::cout << std::endl << "Testing own parallel TS SpGEMM." << std::endl;
    test_SpMM_DSParallelTS(matrix_filename);
    break;
  case GUSTAVSON:
    std::cout << std::endl << "Testing sequential Gustavson SpGEMM."
              << std::endl;
    test_SpMM_sequential(matrix_filename);
    break;
  case LIBRSB:
#ifdef USE_LIBRSB
    std::cout << std::endl << "Testing librsb SpGEMM." << std::endl;
    test_librsb(matrix_filename);
#endif
    break;
  case COMBBLAS:
#ifdef USE_COMBBLAS
    int nprocs, myrank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
    if (!myrank) {
      std::cout << std::endl << "Testing CombBLAS SpGEMM." << std::endl;
    }
    std::cout << "MPI: This is processor rank " << myrank
              << " of " << nprocs << " processes." << std::endl;

    test_CombBLAS(matrix_filename);
    MPI_Finalize();
#endif
    break;
  }
  return 0;
}

static void test_SpMM_sequential(std::string filename)
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
}

static void test_SpMM_DSParallelRS(std::string filename)
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
              << "C = SpMM_DSParallelRS(A, A) ... ";

    clock_gettime(CLOCK_REALTIME, &t1);
    SpMatrix C =
      SpMM_DSParallel_RowStore<excess::concurrent_bag_MSTLB>(A, A);
      //SpMM_DSParallel_RowStore<excess::concurrent_bag_TBBQueue>(A, A);
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

static void test_SpMM_DSParallelTS(std::string filename)
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
              << "C = SpMM_DSParallelTS(A, A) ... ";

    clock_gettime(CLOCK_REALTIME, &t1);
    SpMatrix C =
      SpMM_DSParallel_TripletStore<excess::concurrent_bag_MSTLB>(A, A);
      //SpMM_DSParallel_TripletStore<excess::concurrent_bag_TBBQueue>(A, A);
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
static void test_librsb(std::string filename)
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
    rsb_file_mtx_load(filename.c_str(),
                      RSB_FLAG_NOFLAGS & ~RSB_FLAG_USE_HALFWORD_INDICES,
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
    // NOTE: According to the librsb docs rsb_spmsp ignores both
    //       the transposition and alpha and beta parameters.
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

#ifdef USE_COMBBLAS
static void test_CombBLAS(std::string filename)
{
  typedef SpDCCols < int, double > DCCols;
  typedef SpParMat < int, double, DCCols > MPI_DCColMat;

  int myrank;
  double t1, t2;
  MPI_Comm_rank(MPI_COMM_WORLD,&myrank);

  if (!myrank) {
    std::cout << "Attempting to load the matrix '" << filename
              << "' into A ... ";
  }
  t1 = MPI_Wtime();
  MPI_DCColMat A;
  A.ReadDistribute(filename, 0);
  MPI_Barrier(MPI_COMM_WORLD);
  t2 = MPI_Wtime();
  if (!myrank) {
    std::cout << "Ok." << std::endl;
  }
  A.PrintInfo();
  if (!myrank) {
    std::cout << "Duration: "
              << (t2 - t1)
              << " sec" << std::endl;
  }

  // Reload A into a new matrix B as the matrices must be separate.
  MPI_DCColMat B;
  B.ReadDistribute(filename, 0);

  {
    MPI_Barrier(MPI_COMM_WORLD);
    if (!myrank) {
      std::cout << "Attempting matrix-matrix multiplication "
                << "C = (Mult_AnXBn_DoubleBuffA, A) ... ";
    }

    t1 = MPI_Wtime();
    MPI_DCColMat C =
      Mult_AnXBn_DoubleBuff<PlusTimesSRing<double, double>, double, DCCols>
        (A, B);
    MPI_Barrier(MPI_COMM_WORLD);
    t2 = MPI_Wtime();

    if (!myrank) {
      std::cout << "Ok." << std::endl;
    }
    C.PrintInfo();
    if (!myrank) {
      std::cout << "Duration: "
                << (t2 - t1)
                << " sec" << std::endl;
    }
  }
}
#endif

static void process_arguments(int argc, char** argv)
{
  using std::string;

  int i = 1;
  while (i < argc) {
    string arg = string(argv[i]);
    if (arg.compare("-h") == 0) {
      print_usage(argc, argv);
      exit(0);
    } else if (arg.compare("-a") == 0) {
      int algno;
      int ok = 0;
      if ((++i < argc) && try_parse(string(argv[i]), algno) &&
          (0 <= algno) && (algno < no_algorithms)) {
        algorithm = algorithm_t(algno);
        ok = 1;
      } else {
        std::cerr << "Error: Bad algorithm# given with '-a " << argv[i] << "'."
                  << std::endl;
      }
      if (!ok) {
        print_usage(argc, argv);
        exit(-1);
      }
    } else {
      if (i == argc - 1) {
        matrix_filename = string(argv[i]);
      } else {
        std::cerr << "Error: Unknown argument '" << argv[i] << "'."
                  << std::endl;
        print_usage(argc, argv);
        exit(-1);
      }
    }
    i++;
  }
}

static void print_usage(int argc, char** argv)
{
  using std::cout;
  using std::endl;

  cout << endl;
  cout << "EXCESS sparse matrix multiplication experiment." << endl;
  //cout << "  Copyright (C) 2015  Anders Gidenstam" << endl;

  cout << endl;
  cout << "Usage: " << argv[0] << " [options] <matrix file>" << endl;
  cout << endl;

  cout << "  -h                Print this message and exit." << endl;
  cout << "  -a <algorithm#>   Set the SpGEMM algorithm/library to use."
       << endl;
  cout << "                    <algorithm#> can be one of the following."
       << endl;
  {
    cout << "                      " << "0.  " << "New EXCESS algorithm with row store."
         << endl;
    cout << "                      " << "1.  " << "New EXCESS algorithm with triplet store."
         << endl;
    cout << "                      " << "2.  " << "Sequential Gustavson algorithm."
         << endl;
#ifdef USE_LIBRSB
    cout << "                      " << "3.  " << "librsb."
         << endl;
#endif
#ifdef USE_COMBBLAS
    cout << "                      " << "4.  " << "CombBLAS."
         << endl;
#endif
  }
}

 static bool try_parse(std::string s, int& i)
{
  std::stringstream ss(s);
  char c;
  ss >> i;
  return !(ss.fail() || ss.get(c));
}
