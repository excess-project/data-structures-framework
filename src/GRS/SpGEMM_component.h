// GRS sparse CSR matrix-matrix multiplication component.
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
#ifndef __SPGEMM_GRS_COMP
#define __SPGEMM_GRS_COMP

#include <vector>
#include <iostream>

#include <grs.h>

#include <cstring>

#include <SpMatrix.h>

namespace EXCESS_GRS {

struct matrix_csr
{
  grs_data d_m_n_nzmax, d_rp, d_ci, d_v;
  struct grs_vector* vec_m_n_nzmax; // int vector of length 3.
  struct grs_vector* vec_rp;
  struct grs_vector* vec_ci;
  struct grs_vector* vec_v;

  matrix_csr(int m, int n)
  {
    initialize(m, n, 0);
  }

  matrix_csr(SpMatrix matrix)
  {
    initialize(matrix.m, matrix.n, matrix.nzmax);
    // Copy the matrix contents. 
    std::memcpy(read_grs_vector_on_cpu(vec_rp),
                matrix.rp,
                (matrix.m+1) * sizeof(int));
    std::memcpy(read_grs_vector_on_cpu(vec_ci),
                matrix.ci,
                matrix.nzmax * sizeof(int));
    std::memcpy(read_grs_vector_on_cpu(vec_v),
                matrix.v,
                matrix.nzmax * sizeof(double));
  }

  ~matrix_csr()
  {
    std::cout << "SpGEMM_component.h::~matrix_csr()" << std::endl;
    // NOTE: The actual data blocks should also be freed.
    free(vec_m_n_nzmax);
    free(vec_rp);
    free(vec_ci);
    free(vec_v);
  }

  int rows() const
  {
    int* m_n_nzmax = (int*)read_grs_vector_on_cpu(vec_m_n_nzmax);
    return m_n_nzmax[0];
  }

  int columns() const
  {
    int* m_n_nzmax = (int*)read_grs_vector_on_cpu(vec_m_n_nzmax);
    return m_n_nzmax[1];
  }

  int nonzeros() const
  {
    int* m_n_nzmax = (int*)read_grs_vector_on_cpu(vec_m_n_nzmax);
    return m_n_nzmax[2];
  }

  void save_to_file(std::string filename)
  {
    SpMatrix matrix = convert_to_SpMatrix();
    matrix.SaveToFile(filename);
  }

  // Creates an independent SpMatrix instance.
  SpMatrix convert_to_SpMatrix()
  {
    int* m_n_nzmax = (int*)read_grs_vector_on_cpu(vec_m_n_nzmax);
    SpMatrix matrix = SpMatrix(m_n_nzmax[0],
                               m_n_nzmax[1],
                               m_n_nzmax[2]);
    std::memcpy(matrix.rp,
                read_grs_vector_on_cpu(vec_rp),
                (matrix.m+1) * sizeof(int));
    std::memcpy(matrix.ci,
                read_grs_vector_on_cpu(vec_ci),
                matrix.nzmax * sizeof(int));
    std::memcpy(matrix.v,
                read_grs_vector_on_cpu(vec_v),
                matrix.nzmax * sizeof(double));
    return matrix;
  }

  // Creates a SpMatrix instance that shares its storage with GRS.
  // NOTE: Beware of the SpMatrix destructor! Run clear() on the matrix
  //       before it goes out of scope.
  static void convert_from_grs_input(grs_data** argvin, SpMatrix& matrix)
  {
    // Remove the old matrix content.
    std::free(matrix.rp);
    std::free(matrix.ci);
    std::free(matrix.v);

    grs_vector* vec_m_n_nzmax = (grs_vector*)argvin[0]->d; // matrix m; n; nzmax
    grs_vector* vec_rp = (grs_vector*)argvin[1]->d; // matrix rp
    grs_vector* vec_ci = (grs_vector*)argvin[2]->d; // matrix ci
    grs_vector* vec_v  = (grs_vector*)argvin[3]->d; // matrix v

    // Read m_n_nzmax.
    int* m_n_nzmax = (int*)read_grs_vector_on_cpu(vec_m_n_nzmax);

    // Initialize the SpMatrix using the already existing storage.
    matrix.m = m_n_nzmax[0];
    matrix.n = m_n_nzmax[1];
    matrix.nzmax = m_n_nzmax[2];
    matrix.rp = (int*)read_grs_vector_on_cpu(vec_rp);
    matrix.ci = (int*)read_grs_vector_on_cpu(vec_ci);
    matrix.v  = (double*)read_grs_vector_on_cpu(vec_v);
  }

  // To make a SpMatrix sharing storage with GRS safe for destruction.
  static void clear(SpMatrix& matrix)
  {
    matrix.nzmax = 0;
    matrix.rp = 0;
    matrix.ci = 0;
    matrix.v  = 0;
  }

  static void convert_to_grs_output(SpMatrix& matrix, grs_data** argvout)
  {
    // NOTE: The matrix argument will be destroyed. 

    // argvout mapping:
    //   argvout[0]:  m_n_nzmax
    //   argvout[1]:  rp
    //   argvout[2]:  ci
    //   argvout[3]:  v

    // Set m_n_nzmax using the existing result matrix.
    ((grs_vector*)argvout[0]->d)->mode = 0;
    int* m_n_nzmax = (int*)((grs_vector*)argvout[0]->d)->d_cpu;
    m_n_nzmax[0] = matrix.m;
    m_n_nzmax[1] = matrix.n;
    m_n_nzmax[2] = matrix.nzmax;

    // Set rp using the existing result matrix.
    ((grs_vector*)argvout[1]->d)->mode  = 0;
    ((grs_vector*)argvout[1]->d)->d_cpu = matrix.rp;
    ((grs_vector*)argvout[1]->d)->size  = (matrix.m*sizeof(int))/sizeof(float);

    // Set ci using the existing result matrix.
    ((grs_vector*)argvout[2]->d)->mode  = 0;
    ((grs_vector*)argvout[2]->d)->d_cpu = matrix.ci;
    ((grs_vector*)argvout[2]->d)->size  =
      (matrix.nzmax*sizeof(int))/sizeof(float);

    // Set v using the existing result matrix.
    ((grs_vector*)argvout[3]->d)->mode  = 0;
    ((grs_vector*)argvout[3]->d)->d_cpu = matrix.v;
    ((grs_vector*)argvout[3]->d)->size  =
      (matrix.nzmax*sizeof(double))/sizeof(float);

    clear(matrix);
  }

private:
  void initialize(int m, int n, int nzmax)
  {
    int* m_n_nzmax = (int*)malloc(3*sizeof(int));
    m_n_nzmax[0] = m;
    m_n_nzmax[1] = n;
    m_n_nzmax[2] = nzmax;

    grs_data_init(&d_m_n_nzmax, GRS_VECTOR);
    grs_data_init(&d_rp, GRS_VECTOR);
    grs_data_init(&d_ci, GRS_VECTOR);
    grs_data_init(&d_v,  GRS_VECTOR);

    vec_m_n_nzmax = (struct grs_vector*)malloc(sizeof(struct grs_vector));
    vec_rp        = (struct grs_vector*)malloc(sizeof(struct grs_vector));
    vec_ci        = (struct grs_vector*)malloc(sizeof(struct grs_vector));
    vec_v         = (struct grs_vector*)malloc(sizeof(struct grs_vector));
    
    grs_data_set_vector(&d_m_n_nzmax, vec_m_n_nzmax);
    grs_data_set_vector(&d_rp, vec_rp);
    grs_data_set_vector(&d_ci, vec_ci);
    grs_data_set_vector(&d_v,  vec_v);

    grs_vector_init(vec_m_n_nzmax, (float*)m_n_nzmax, 4, "m_n_nzmax");
    grs_vector_init(vec_rp, (float*)malloc((m+1)*sizeof(int)),
                    (m*sizeof(int))/sizeof(float), "rp");
    grs_vector_init(vec_ci, (float*)malloc(nzmax*sizeof(int)),
                    (nzmax*sizeof(int))/sizeof(float), "ci");
    grs_vector_init(vec_v,  (float*)malloc(nzmax*sizeof(double)),
                    (nzmax*sizeof(double))/sizeof(float), "v");
  }

  static float* read_grs_vector_on_cpu(grs_vector* vec_x)
  {
    float* x = (float*)vec_x->d_cpu;
    int size = vec_x->size;

    if(vec_x->mode==1) {
      std::cout << "read_grs_vector_on_cpu: Reading stale data on CPU. "
                << "FIXME. Should refresh from GPU." << std::endl;
      //copy_from_gpu(in1, (float*) input1->d_gpu, ode_size);
      vec_x->mode = 0;
    }
    return x;
  }
};

class SpGEMM_component_csr
{
public:
  class variant
  {
  public:
    variant() {};
    virtual ~variant() {};
  private:
    // Disallow copying and assignment.
    variant(const variant&);
    void operator=(const variant&);
  };

  SpGEMM_component_csr();
  ~SpGEMM_component_csr();

  void setup_call(struct grs_call* call,
                  std::string      name,
                  matrix_csr&      A,
                  matrix_csr&      B,
                  matrix_csr&      Result);

private:
  // Disallow copying and assignment.
  SpGEMM_component_csr(const SpGEMM_component_csr&);
  void operator=(const SpGEMM_component_csr&);

  struct grs_component      this_component;
  std::vector< variant* >   variants;
};

}
#endif
