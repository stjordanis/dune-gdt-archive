#ifndef DUNE_DETAILED_DISCRETIZATIONS_EVALUATION_LOCAL_QUATERNARY_IPDGFLUXES_HH
#define DUNE_DETAILED_DISCRETIZATIONS_EVALUATION_LOCAL_QUATERNARY_IPDGFLUXES_HH

#include <memory>

#include <dune/common/densematrix.hh>

#include <dune/stuff/function/expression.hh>

namespace Dune {

namespace Detailed {

namespace Discretizations {

namespace Evaluation {

namespace Local {

namespace Quaternary {

namespace IPDGfluxes {

/**
  \todo       Implement penalty parameter
  \todo       Implement different constructor, for function and discretefunction
  **/
template< class FunctionSpaceImp,
          class InducingFunctionImp = Dune::Stuff::FunctionExpression< typename FunctionSpaceImp::DomainFieldType, FunctionSpaceImp::DimDomain, typename FunctionSpaceImp::RangeFieldType, FunctionSpaceImp::DimRange > >
class Inner
{
public:
  typedef FunctionSpaceImp FunctionSpaceType;

  typedef InducingFunctionImp InducingFunctionType;

  typedef Inner< FunctionSpaceType, InducingFunctionType > ThisType;

  typedef typename FunctionSpaceType::DomainType DomainType;

  typedef typename FunctionSpaceType::RangeType RangeType;

  typedef typename FunctionSpaceType::RangeFieldType RangeFieldType;

  typedef typename FunctionSpaceType::JacobianRangeType JacobianRangeType;

  Inner(const std::shared_ptr< const InducingFunctionType > inducingFunction,
        const unsigned int order,
        const RangeFieldType penaltyFactor)
    : inducingFunction_(inducingFunction)
    , order_(order)
    , penaltyFactor_(penaltyFactor)
  {}

  //! returns the inducing function
  const std::shared_ptr< const InducingFunctionType > inducingFunction() const
  {
    return inducingFunction_;
  }

  unsigned int order() const
  {
    return order_;
  }

  /**
    \attention  Assumes, that the return matrices are empty, because we do multiple +=
    \todo       There are currently four nested loops, these can be simplyfied to two (see msdg-matlab)
    \todo       Rename Entity -> En, Neighbor -> Ne (otherwise stuff is too long)
    **/
  template< class LocalAnsatzBaseFunctionSetEntityType,
            class LocalTestBaseFunctionSetEntityType,
            class LocalAnsatzBaseFunctionSetNeighborType,
            class LocalTestBaseFunctionSetNeighborType,
            class IntersectionType,
            class LocalPointType,
            class LocalMatrixImp >
  void evaluateLocal( const LocalAnsatzBaseFunctionSetEntityType& localAnsatzBaseFunctionSetEntity,
                      const LocalTestBaseFunctionSetEntityType& localTestBaseFunctionSetEntity,
                      const LocalAnsatzBaseFunctionSetNeighborType& localAnsatzBaseFunctionSetNeighbor,
                      const LocalTestBaseFunctionSetNeighborType& localTestBaseFunctionSetNeighbor,
                      const IntersectionType& intersection,
                      const LocalPointType& localPoint,
                      Dune::DenseMatrix< LocalMatrixImp >& entityEntityRet,
                      Dune::DenseMatrix< LocalMatrixImp >& NeighborNeighborRet,
                      Dune::DenseMatrix< LocalMatrixImp >& entityNeighborRet,
                      Dune::DenseMatrix< LocalMatrixImp >& NeighborEntityRet) const
  {
    // some stuff
//    std::cout << "========================================================" << std::endl;
//    std::cout << "localPoint = " << localPoint << std::endl;
    const DomainType globalPoint = intersection.geometry().global(localPoint);
//    std::cout << "globalPoint = " << globalPoint << std::endl;
    const DomainType localPointEntity = intersection.geometryInInside().global(localPoint);
//    std::cout << "localPointEntity = " << localPointEntity << std::endl;
    const DomainType localPointNeighbor = intersection.geometryInOutside().global(localPoint);
//    std::cout << "localPointNeighbor = " << localPointNeighbor << std::endl;
    const DomainType unitOuterNormal = intersection.unitOuterNormal(localPoint);
    const RangeType zeroRange(0.0);
    const JacobianRangeType zeroJacobainRange(0.0);
    RangeFieldType gradientTimesNormal(0.0);
    RangeType gradientTimesNormalTimesEvaluation(0.0);

    // evaluate ansatz entity basefunctionset
    const unsigned int rowsEntity = localAnsatzBaseFunctionSetEntity.size();
    std::vector< RangeType > localAnsatzBaseFunctionSetEntityEvaluations(rowsEntity, zeroRange);
    std::vector< JacobianRangeType > localAnsatzBaseFunctionSetEntityGradients(rowsEntity, zeroJacobainRange);
    localAnsatzBaseFunctionSetEntity.evaluate(localPointEntity, localAnsatzBaseFunctionSetEntityEvaluations);
//    print_info("phi_j_entity", localAnsatzBaseFunctionSetEntityEvaluations);
    localAnsatzBaseFunctionSetEntity.jacobian(localPointEntity, localAnsatzBaseFunctionSetEntityGradients);
//    print_info("grad_phi_j_entity", localAnsatzBaseFunctionSetEntityGradients);

    // evaluate ansatz Neighbor basefunctionset
    const unsigned int rowsNeighbor = localAnsatzBaseFunctionSetNeighbor.size();
    std::vector< RangeType > localAnsatzBaseFunctionSetNeighborEvaluations(rowsNeighbor, zeroRange);
    std::vector< JacobianRangeType > localAnsatzBaseFunctionSetNeighborGradients(rowsNeighbor, zeroJacobainRange);
    localAnsatzBaseFunctionSetNeighbor.evaluate(localPointNeighbor, localAnsatzBaseFunctionSetNeighborEvaluations);
//    print_info("phi_j_neighbor", localAnsatzBaseFunctionSetNeighborEvaluations);
    localAnsatzBaseFunctionSetNeighbor.jacobian(localPointNeighbor, localAnsatzBaseFunctionSetNeighborGradients);
//    print_info("grad_phi_j_neighbor", localAnsatzBaseFunctionSetNeighborGradients);

    // evaluate test entity basefunctionset
    const unsigned int colsEntity = localTestBaseFunctionSetEntity.size();
    std::vector< RangeType > localTestBaseFunctionSetEntityEvaluations(colsEntity, zeroRange);
    std::vector< JacobianRangeType > localTestBaseFunctionSetEntityGradients(colsEntity, zeroJacobainRange);
    localTestBaseFunctionSetEntity.evaluate(localPointEntity, localTestBaseFunctionSetEntityEvaluations);
//    print_info("phi_i_entity", localTestBaseFunctionSetEntityEvaluations);
    localTestBaseFunctionSetEntity.jacobian(localPointEntity, localTestBaseFunctionSetEntityGradients);
//    print_info("grad_phi_i_entity", localTestBaseFunctionSetEntityGradients);

    // evaluate test Neighbor basefunctionset
    const unsigned int colsNeighbor = localTestBaseFunctionSetNeighbor.size();
    std::vector< RangeType > localTestBaseFunctionSetNeighborEvaluations(colsNeighbor, zeroRange);
    std::vector< JacobianRangeType > localTestBaseFunctionSetNeighborGradients(colsNeighbor, zeroJacobainRange);
    localTestBaseFunctionSetNeighbor.evaluate(localPointNeighbor, localTestBaseFunctionSetNeighborEvaluations);
//    print_info("phi_i_neighbor", localTestBaseFunctionSetNeighborEvaluations);
    localTestBaseFunctionSetNeighbor.jacobian(localPointNeighbor, localTestBaseFunctionSetNeighborGradients);
//    print_info("grad_phi_i_neighbor", localTestBaseFunctionSetNeighborGradients);

    // evaluate inducing function
    RangeType functionValue(0.0);
    inducingFunction_->evaluate(globalPoint, functionValue);
//    std::cout << "functionValue: " << functionValue << std::endl;

    // evaluate penalty parameter
    const RangeFieldType penaltyParameter = penaltyFactor_ / std::pow(intersection.geometry().volume(), 1.0);
//    std::cout << "penaltyFactor_: " << penaltyFactor_ << std::endl;
//    std::cout << "intersection.geometry().volume(): " << intersection.geometry().volume() << std::endl;
//    std::cout << "penaltyParameter: " << penaltyParameter << std::endl;

    // entity entity combinations
    assert(entityEntityRet.rows() == rowsEntity);
    assert(entityEntityRet.cols() == colsEntity);
    // loop over all ansatz functions
    for(unsigned int i = 0; i < rowsEntity; ++i ) {
      // get row of ret matrix
      typename Dune::DenseMatrix< LocalMatrixImp >::row_reference entityEntityRetRow = entityEntityRet[i];
      // loop over all test function
      for( unsigned int j = 0; j < colsEntity; ++j ) { {
          gradientTimesNormal = localTestBaseFunctionSetEntityGradients[j][0] * unitOuterNormal;
          gradientTimesNormalTimesEvaluation = localAnsatzBaseFunctionSetEntityEvaluations[i] * gradientTimesNormal;
          entityEntityRetRow[j] += -0.5 * functionValue * gradientTimesNormalTimesEvaluation;
        } {
          const RangeFieldType normalTimesGradient = unitOuterNormal * localAnsatzBaseFunctionSetEntityGradients[i][0];
          const RangeType evalautionTimesNormalTimesGradient = localTestBaseFunctionSetEntityEvaluations[j] * normalTimesGradient;
          entityEntityRetRow[j] += -0.5 * functionValue * evalautionTimesNormalTimesGradient;
        } {
          const RangeFieldType evalautionTimesEvaluation = localAnsatzBaseFunctionSetEntityEvaluations[i] * localTestBaseFunctionSetEntityEvaluations[j];
          entityEntityRetRow[j] += penaltyParameter * evalautionTimesEvaluation;
      } } // loop over all test function
    } // loop over all ansatz functions

    // do entity neighbor combinations
    assert( entityNeighborRet.rows() == rowsEntity );
    assert( entityNeighborRet.cols() == colsNeighbor );
    for( unsigned int i = 0; i < rowsEntity; ++i )
    {
      for( unsigned int j = 0; j < colsNeighbor; ++j )
      {
        {
          gradientTimesNormal = localTestBaseFunctionSetNeighborGradients[j][0] * unitOuterNormal;
          const RangeType evaluationTimesGradientTimesNormal = localAnsatzBaseFunctionSetEntityEvaluations[i] * gradientTimesNormal;
          entityNeighborRet[i][j] += -0.5 * functionValue * evaluationTimesGradientTimesNormal;
        }
        {
          const RangeFieldType normalTimesGradient = unitOuterNormal * localAnsatzBaseFunctionSetEntityGradients[i][0];
          const RangeType evaluationTimesNormalTimesGradient = localTestBaseFunctionSetNeighborEvaluations[j] * normalTimesGradient;
          entityNeighborRet[i][j] += 0.5 * functionValue * evaluationTimesNormalTimesGradient;
        }
        {
          const RangeFieldType evaluationTimesEvaluation = localTestBaseFunctionSetNeighborEvaluations[j] * localAnsatzBaseFunctionSetEntityEvaluations[i];
          entityNeighborRet[i][j] += -1.0 * penaltyParameter * evaluationTimesEvaluation;
        }
      }
    } // done entity Neighbor combinations

    // do Neighbor entity combinations
    assert( NeighborEntityRet.rows() == rowsNeighbor );
    assert( NeighborEntityRet.cols() == colsEntity );
    for( unsigned int i = 0; i < rowsNeighbor; ++i )
    {
      for( unsigned int j = 0; j < colsEntity; ++j )
      {
        {
          gradientTimesNormal = localTestBaseFunctionSetEntityGradients[j][0] * unitOuterNormal;
          gradientTimesNormalTimesEvaluation = localAnsatzBaseFunctionSetNeighborEvaluations[i] * gradientTimesNormal;
          NeighborEntityRet[i][j] += 0.5 * functionValue * gradientTimesNormalTimesEvaluation;
        }
        {
          const RangeFieldType normalTimesGradient = unitOuterNormal * localAnsatzBaseFunctionSetNeighborGradients[i][0];
          const RangeType evaluationTimesNormalTimesGradient = localTestBaseFunctionSetEntityEvaluations[j] * normalTimesGradient;
          NeighborEntityRet[i][j] += -0.5 * functionValue * evaluationTimesNormalTimesGradient;
        }
        {
          const RangeFieldType evaluationTimesEvalaution = localTestBaseFunctionSetEntityEvaluations[j] * localAnsatzBaseFunctionSetNeighborEvaluations[i];
          NeighborEntityRet[i][j] += -1.0 * penaltyParameter * evaluationTimesEvalaution;
        }
      }
    } // done Neighbor entity combinations

    // do Neighbor Neighbor combinations
    assert( NeighborNeighborRet.rows() == rowsNeighbor );
    assert( NeighborNeighborRet.cols() == colsNeighbor );
    for( unsigned int i = 0; i < rowsNeighbor; ++i )
    {
      for( unsigned int j = 0; j < colsNeighbor; ++j )
      {
        {
          gradientTimesNormal = localTestBaseFunctionSetNeighborGradients[j][0] * unitOuterNormal;
          const RangeType gradientTimesNormalTimesEvalaution = localAnsatzBaseFunctionSetNeighborEvaluations[i] * gradientTimesNormal;
          NeighborNeighborRet[i][j] += 0.5 * functionValue * gradientTimesNormalTimesEvalaution;
        }
        {
          const RangeFieldType normalTimesGradient = unitOuterNormal * localAnsatzBaseFunctionSetNeighborGradients[i][0];
          const RangeType evaluationTimesNormalTimesGradient = localTestBaseFunctionSetNeighborEvaluations[j] * normalTimesGradient;
          NeighborNeighborRet[i][j] += 0.5 * functionValue * evaluationTimesNormalTimesGradient;
        }
        {
          const RangeFieldType evaluationTimesEvaluation = localTestBaseFunctionSetNeighborEvaluations[j] * localAnsatzBaseFunctionSetNeighborEvaluations[i];
          NeighborNeighborRet[i][j] += penaltyParameter * evaluationTimesEvaluation;
        }
      }
    } // done Neighbor Neighbor combinations

  } // end method evaluateLocal

private:
//  template< class VectorImp >
//  void print_info(const std::string name, const VectorImp& vector) const
//  {
//    for (unsigned int i = 0; i < vector.size(); ++i) {
//      std::cout << name << "^" << i << ": " << vector[i] << std::endl;
//    }
//  }

  Inner(const ThisType&);
  ThisType& operator=(const ThisType&);

  const std::shared_ptr< const InducingFunctionType > inducingFunction_;
  const unsigned int order_;
  const RangeFieldType penaltyFactor_;
}; // end class Inner

} // end namespace IPDGfluxes

} // end namespace Quaternary

} // end namespace Local

} // end namespace Evaluation

} // namespace Discretizations

} // namespace Detailed

} // end namespace Dune

#endif // DUNE_DETAILED_DISCRETIZATIONS_EVALUATION_LOCAL_QUATERNARY_IPDGFLUXES_HH