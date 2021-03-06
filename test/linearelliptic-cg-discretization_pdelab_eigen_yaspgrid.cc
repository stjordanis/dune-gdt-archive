// This file is part of the dune-gdt project:
//   http://users.dune-project.org/projects/dune-gdt
// Copyright holders: Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_STUFF_TEST_MAIN_ENABLE_INFO_LOGGING
# define DUNE_STUFF_TEST_MAIN_ENABLE_INFO_LOGGING 1
#endif

// This one has to come first (includes the config.h)!
#include <dune/stuff/test/main.hxx>

#include "linearelliptic-cg-discretization.hh"

using namespace Dune;
using namespace Dune::GDT;


#if HAVE_DUNE_PDELAB && HAVE_EIGEN

TYPED_TEST_CASE(linearelliptic_CG_discretization, YaspGridTestCases);
TYPED_TEST(linearelliptic_CG_discretization, eoc_study_using_pdelab_and_eigen_and_sgrid) {
  this->template eoc_study< ChooseSpaceBackend::pdelab, Stuff::LA::ChooseBackend::eigen_sparse >();
}

#else

TEST(DISABLED_linearelliptic_CG_discretization, eoc_study_using_pdelab_and_eigen_and_sgrid) {}

#endif
