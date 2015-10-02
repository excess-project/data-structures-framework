// Sparse double matrix for sparse BLAS type of operations.
//   Copyright (C) 2015  Anders Gidenstam

#ifndef __SPMATRIX_H
#define __SPMATRIX_H

#include <string>
#include <vector>

// Sparse matrix in CSR format.
class SpMatrix
{
public: // For now.
  int     nzmax;  // max #non-zeros
  int     m;      // #rows
  int     n;      // #columns
  int*    rp;     // row pointer array, indices 0 to m
  int*    ci;     // column index array, indices 0 to nz-1
  double* v;      // values, indices 0 to nz-1

public:
  typedef std::pair< int,int > RowColumnPair_t;
  typedef std::pair< RowColumnPair_t, double > MatrixTriple_t;
  struct MatrixRow_t {
    int     row;
    int     nzmax;
    int*    ci;
    double* v;
    
    MatrixRow_t(int row, int nzmax);
    ~MatrixRow_t();
  };

  SpMatrix(int rows, int columns);
  SpMatrix(int rows, int columns, int nzmax);
  SpMatrix(const SpMatrix& src);
  SpMatrix(int rows, int columns, std::vector<MatrixTriple_t> Mij);
  // NOTE: Assumes the entries are provided in row-major order.

  ~SpMatrix();

  SpMatrix& operator= (const SpMatrix& other);

  static SpMatrix LoadFromFile(std::string filename);

private:
  void Init(int rows, int columns, int nzmax);
};

bool RowMajorLessThan(SpMatrix::MatrixTriple_t& a,
                      SpMatrix::MatrixTriple_t& b);

#endif
