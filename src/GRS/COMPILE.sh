#!/bin/bash

g++ -c -g -fopenmp -DUSE_OPENMP {../../../grs_0.09_snap/grs,SpMMTest}.cpp -I $CUDA_DIR/include/ -I ../SpBLAS -I ../include -I ../../../grs_0.09_snap -I .  -Wno-write-strings && \
g++ -c -g -fopenmp -DUSE_OPENMP {SpGEMM_component,SpGEMM_variant_sequential,SpGEMM_variant_DSParallel}.cpp ../SpBLAS/{SpMatrix,SparseAccumulator}.cpp -I $CUDA_DIR/include/ -I ../SpBLAS -I ../include -I ../../../grs_0.09_snap -I .  -Wno-write-strings && \
nvcc -g -o grs_spmmtest {SpMMTest,grs,help_cuda,SpGEMM_component,SpGEMM_variant_sequential,SpGEMM_variant_DSParallel,SpMatrix,SparseAccumulator}.o -Xcompiler -fopenmp

