#ifndef DUNE_DETAILED_DISCRETIZATIONS_CONTAINER_FACTORY_EIGEN_HH
#define DUNE_DETAILED_DISCRETIZATIONS_CONTAINER_FACTORY_EIGEN_HH

//#ifdef HAVE_EIGEN

// system
#include <vector>
#include <set>

// dune-common
#include <dune/common/shared_ptr.hh>

// local
#include "../backend/container/eigen.hh"

namespace Dune
{

namespace Detailed {

namespace Discretizations
{

namespace LA
{

namespace Factory
{

//! \attention There might be a problem with the sparsity pattern.
//! \todo Understand the eigen sparsity pattern and which insertion method to use!
template< class EntryType >
class Eigen
{
public:
  typedef Dune::Detailed::Discretizations::LA::Backend::Container::Eigen::SparseMatrix< EntryType > SparseMatrixType;

  typedef Dune::Detailed::Discretizations::LA::Backend::Container::Eigen::DenseMatrix< EntryType > DenseMatrixType;

  typedef Dune::Detailed::Discretizations::LA::Backend::Container::Eigen::DenseVector< EntryType > DenseVectorType;

  typedef std::map< unsigned int, std::set< unsigned int > > PatternType;

  template< class AnsatzSpaceType, class TestSpaceType >
  static SparseMatrixType createSparseMatrix(const AnsatzSpaceType& ansatzSpace, const TestSpaceType& testSpace)
  {
    // generate sparsity pattern
    const PatternType pattern = ansatzSpace.computePattern(testSpace);
    return createSparseMatrix(ansatzSpace.map().size(), testSpace.map().size(), pattern);
  } // static SparseMatrixType createSparseMatrix(const AnsatzSpaceType& ansatzSpace, const TestSpaceType& testSpace)

  static SparseMatrixType createSparseMatrix(const unsigned int rows, const unsigned int cols, const PatternType& pattern)
  {
    // init
    SparseMatrixType matrix(rows, cols);

    // tell pattern to matrix
    for (typename PatternType::const_iterator rowSet = pattern.begin(); rowSet != pattern.end(); ++rowSet) {
      const unsigned int row = rowSet->first;
      const std::set< unsigned int >& rowEntries = rowSet->second;
//      matrix.storage()->startVec(row);
      for (typename std::set< unsigned int >::iterator rowEntry = rowEntries.begin();
          rowEntry != rowEntries.end(); ++rowEntry) {
        unsigned int column = *rowEntry;
        matrix.storage()->insert/*BackByOuterInner*/(row, column);
      }
    } // tell pattern to matrix

    // finalize matrix
    matrix.storage()->finalize();
    matrix.storage()->makeCompressed();

    // return
    return matrix;
  } // static SparseMatrixType createSparseMatrix(const AnsatzSpaceType& ansatzSpace, const TestSpaceType& testSpace)

  template< class AnsatzSpaceType, class TestSpaceType >
  static DenseMatrixType createDenseMatrix(const AnsatzSpaceType& ansatzSpace, const TestSpaceType& testSpace)
  {
    return createDenseMatrix(ansatzSpace.map().size(), testSpace.map().size());
  }

  static DenseMatrixType createDenseMatrix(const unsigned int rows, const unsigned int cols)
  {
    // init
    DenseMatrixType matrix(rows, cols);
    // reserve
    matrix.reserve();
    // return
    return matrix;
  } // static DenseMatrixType createDenseMatrix(const AnsatzSpaceType& ansatzSpace, const TestSpaceType& testSpace)

  template< class SpaceType >
  static DenseVectorType createDenseVector(const SpaceType& space)
  {
    return createDenseVector(space.map().size());
  }

  static DenseVectorType createDenseVector(const unsigned int size)
  {
    // init
    DenseVectorType vector(size);
    // reserve
    vector.reserve();
    // return
    return vector;
  } // static DenseVectorType createDenseVector(const SpaceType& space)
}; // class Eigen

} // namespace Factory

} // namespace LA

} // namespace Discretizations

} // namespace Detailed

} // namespace Dune

//#endif // HAVE_EIGEN

#endif // DUNE_DETAILED_DISCRETIZATIONS_CONTAINER_FACTORY_EIGEN_HH