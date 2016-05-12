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

#include "SpGEMM_component.h"

#include "SpGEMM_variant_sequential.h"
#include "SpGEMM_variant_DSParallel.h"

namespace EXCESS_GRS {

SpGEMM_component_csr::SpGEMM_component_csr()
{
  grs_component_init(&this_component);
  grs_component_set_name(&this_component, "SpGEMM_component_csr");
  // Add all available variants here?
  // NOTE: Currently exactly 2 variants are supported where variant 1 is
  //       assumed to be GPU based.
  //variants.push_back(new SpGEMM_variant_csr_sequential(&this_component, 0));
  variants.push_back(new SpGEMM_variant_csr_dsparallel(&this_component, 0));
  variants.push_back(new SpGEMM_variant_csr_sequential(&this_component, 1));
}

SpGEMM_component_csr::~SpGEMM_component_csr()
{
  grs_component_destroy(&this_component);
}

void
SpGEMM_component_csr::setup_call(struct grs_call* call,
                                 std::string      name,
                                 matrix_csr&      A,
                                 matrix_csr&      B,
                                 matrix_csr&      Result)
{
  grs_call_init(call, &this_component);
  grs_call_set_name(call, (char*)name.c_str());
  // Setup the call arguments.
  // Input matrix A
  grs_call_add_input_opnd(call, &A.d_m_n_nzmax);
  grs_call_add_input_opnd(call, &A.d_rp);
  grs_call_add_input_opnd(call, &A.d_ci);
  grs_call_add_input_opnd(call, &A.d_v);
  // Input matrix B
  grs_call_add_input_opnd(call, &B.d_m_n_nzmax);
  grs_call_add_input_opnd(call, &B.d_rp);
  grs_call_add_input_opnd(call, &B.d_ci);
  grs_call_add_input_opnd(call, &B.d_v);
  // Output matrix C
  grs_call_add_output_opnd(call, &Result.d_m_n_nzmax);
  grs_call_add_output_opnd(call, &Result.d_rp);
  grs_call_add_output_opnd(call, &Result.d_ci);
  grs_call_add_output_opnd(call, &Result.d_v);
  Result.grs_dirty = 1;
}

}
