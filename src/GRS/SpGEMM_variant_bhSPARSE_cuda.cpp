// GRS GPU based sparse CSR matrix-matrix multiplication variant.
// The implementation depends on the bhSPARSE CSR implementation
// from https://github.com/bhSPARSE/Benchmark_SpGEMM_using_CSR
// (MIT License) published in
// [Weifeng Liu and Brian Vinter, "An Efficient GPU General Sparse
// Matrix-Matrix Multiplication for Irregular Data".
// Parallel and Distributed Processing Symposium, 2014 IEEE 28th International
// (IPDPS '14), pp.370-381, 19-23 May 2014.]
// and
// [Weifeng Liu and Brian Vinter, "A Framework for General Sparse
// Matrix-Matrix Multiplication on GPUs and Heterogeneous Processors". Journal
// of Parallel and Distributed Computing (JPDC), pp.47-61, Volume 85,
// November 2015.]
//
// Copyright (C) 2016  Anders Gidenstam
//
// This program is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along with
// this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "SpGEMM_variant_bhSPARSE_cuda.h"

#include <ctime>

#include <bhsparse.h>

namespace EXCESS_GRS {

static void   perform_op(int argcin, int argcout,
                         grs_data** argvin, grs_data** argvout);
static double predict_cost(int argcin, int* in_state, int* in_size);
static double predict_switch(int argcin, int* in_state, int* in_size);

// Module global as the C style callback functions cannot access the
// instance members. Could perhaps be a class member instead?
static bool     cudaInitialized = false;
static bhsparse spgemm;

SpGEMM_variant_csr_bhSPARSE_cuda::
  SpGEMM_variant_csr_bhSPARSE_cuda(struct grs_component* component, int variant)
{
  grs_component_add_variant(component, &this_variant);
  grs_variant_init(&this_variant, perform_op, variant,
                   predict_cost, predict_switch);
  if (!cudaInitialized) {
    bool platforms[NUM_PLATFORMS];
    platforms[BHSPARSE_CUDA] = true;
    if (spgemm.initPlatform(platforms) != BHSPARSE_SUCCESS) {
      std::cout << ("SpGEMM_variant_bhSPARSE_cuda::"
                    "SpGEMM_variant_csr_bhSPARSE_cuda: Error: "
                    "Failed to initialize Cuda platform.")
                << std::endl;
    }
    cudaInitialized = true;
  }
}

SpGEMM_variant_csr_bhSPARSE_cuda::~SpGEMM_variant_csr_bhSPARSE_cuda()
{
  if (cudaInitialized) {
    spgemm.freePlatform();
    cudaInitialized = false;
  }
  grs_variant_destroy(&this_variant);
}

static void perform_op(int argcin, int argcout,
                       grs_data** argvin, grs_data** argvout)
{
  std::cout << "SpGEMM_variant_bhSPARSE_cuda::perform_op started." << std::endl;
  if (argcin != 8 || argcout != 4) {
    std::cout << ("SpGEMM_variant_bhSPARSE_cuda::perform_op: Error: "
                  "Wrong number of arguments.")
              << std::endl;
    return;
  }

  // FIXME: The matrices start on the CPU here.
  //        It is not clear how to interface efficiently with bhsparse.
  matrix_csr A = matrix_csr(argvin);
  matrix_csr B = matrix_csr(argvin + 4);
  int*       Crp = (int*)calloc((A.rows()+1), sizeof(int));

  {
    struct timespec t1, t2; 
    std::cout << "Attempting matrix matrix multiplication "
              << (A.matrix->m) << "x" << (A.matrix->n) << " * "
              << (B.matrix->m) << "x" << (B.matrix->n) << " ... ";

    clock_gettime(CLOCK_REALTIME, &t1);

    if (spgemm.initData(A.rows(), A.columns(), B.columns(),
                        A.nonzeros(), A.matrix->v, A.matrix->rp, A.matrix->ci,
                        B.nonzeros(), B.matrix->v, B.matrix->rp, B.matrix->ci,
                        Crp) != BHSPARSE_SUCCESS) {
      std::cout << ("SpGEMM_variant_bhSPARSE_cuda::perform_op: Error: "
                    "Failed to setup bhSPARSE SpGEMM instance.")
                << std::endl;
    }

    if (spgemm.spgemm() != BHSPARSE_SUCCESS) {
      std::cout << ("SpGEMM_variant_bhSPARSE_cuda::perform_op: Error: "
                    "bhSPARSE SpGEMM spgemm() failed.")
                << std::endl;
    }

    // Read C back to CPU.
    // FIXME: Should be delayed until necessary.
    int nnzC = spgemm.get_nnzC();
    SpMatrix C = SpMatrix(A.rows(), B.columns(), nnzC);

    // Transfer the Crp array to the SpMatrix.
    std::free(C.rp);
    C.rp = Crp;

    // Read the GPU-side ci and v arrays into the SpMatrix.
    if (spgemm.get_C(C.ci, C.v) != BHSPARSE_SUCCESS) {
      std::cout << ("SpGEMM_variant_bhSPARSE_cuda::perform_op: Error: "
                    "bhSPARSE SpGEMM get_C() failed.")
                << std::endl;
    }
    if (spgemm.free_mem() != BHSPARSE_SUCCESS) {
      std::cout << ("SpGEMM_variant_bhSPARSE_cuda::perform_op: Error: "
                    "bhSPARSE SpGEMM free_mem() failed.")
                << std::endl;
    }

    clock_gettime(CLOCK_REALTIME, &t2);

    std::cout << "Ok." << std::endl;
    std::cout << "% Result C is " << (C.m) << "x" << (C.n) << " matrix with "
              << (C.nzmax) << " non-zeros." << std::endl;
    std::cout << "% Duration: "
              << ((double)(t2.tv_sec - t1.tv_sec) +
                  1e-9 * (double)(t2.tv_nsec - t1.tv_nsec))
              << " sec" << std::endl;
    matrix_csr::convert_to_grs_output(C, argvout);
  }
  std::cout << "SpGEMM_variant_bhSPARSE_cuda::perform_op finished."
            << std::endl;
}

static double predict_cost(int argcin, int* in_state, int* in_size)
{
  // FIXME.
  return 4.0;
}

static double predict_switch(int argcin, int* in_state, int* in_size)
{
  // FIXME.
  return 0.0;
}

}
