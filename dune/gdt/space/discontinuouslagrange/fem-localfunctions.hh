// This file is part of the dune-gdt project:
//   http://users.dune-project.org/projects/dune-gdt
// Copyright holders: Felix Albrecht
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_GDT_SPACE_DISCONTINUOUSLAGRANGE_FEM_LOCALFUNCTIONS_HH
#define DUNE_GDT_SPACE_DISCONTINUOUSLAGRANGE_FEM_LOCALFUNCTIONS_HH

#include <dune/common/static_assert.hh>
#include <dune/common/typetraits.hh>
#include <dune/common/exceptions.hh>

#include <dune/grid/sgrid.hh>
#include <dune/grid/yaspgrid.hh>
#include <dune/grid/alugrid.hh>

#include <dune/localfunctions/lagrange/equidistantpoints.hh>
#include <dune/localfunctions/lagrange.hh>

#include <dune/fem/space/common/allgeomtypes.hh>

#include <dune/fem_localfunctions/localfunctions/transformations.hh>
#include <dune/fem_localfunctions/basefunctions/genericbasefunctionsetstorage.hh>
#include <dune/fem_localfunctions/basefunctionsetmap/basefunctionsetmap.hh>
#include <dune/fem_localfunctions/space/genericdiscretefunctionspace.hh>

#include <dune/stuff/common/color.hh>

#include "../../mapper/fem.hh"
#include "../../basefunctionset/fem-localfunctions.hh"
#include "../constraints.hh"
#include "../interface.hh"

namespace Dune {
namespace GDT {
namespace DiscontinuousLagrangeSpace {


// forward, to be used in the traits and to allow for specialization
template< class GridPartImp, int polynomialOrder, class RangeFieldImp, int rangeDim, int rangeDimCols = 1 >
class FemLocalfunctionsWrapper;


// forward, to allow for specialization
template< class GridPartImp, int polynomialOrder, class RangeFieldImp, int rangeDim, int rangeDimCols = 1 >
class FemLocalfunctionsWrapperTraits;


/**
 *  \brief Traits class for ContinuousLagrangeSpace for dimRange 1x1.
 */
template< class GridPartImp, int polynomialOrder, class RangeFieldImp >
class FemLocalfunctionsWrapperTraits< GridPartImp, polynomialOrder, RangeFieldImp, 1, 1 >
{
public:
  typedef GridPartImp                   GridPartType;
  static const int                      polOrder = polynomialOrder;
  dune_static_assert((polOrder >= 1), "ERROR: wrong polOrder given!");
private:
  typedef typename GridPartType::ctype  DomainFieldType;
  static const unsigned int             dimDomain = GridPartType::dimension;
public:
  typedef RangeFieldImp                 RangeFieldType;
  static const unsigned int             dimRange = 1;
  static const unsigned int             dimRangeCols = 1;
  typedef FemLocalfunctionsWrapper< GridPartType, polOrder, RangeFieldType, dimRange, dimRangeCols > derived_type;
  typedef Dune::LagrangeLocalFiniteElement< Dune::EquidistantPointSet,
                                            dimDomain,
                                            DomainFieldType,
                                            RangeFieldType > ContinuousFiniteElementType;
  typedef Dune::DGLocalFiniteElement< ContinuousFiniteElementType > FiniteElementType;
private:
  typedef Dune::FemLocalFunctions::BaseFunctionSetMap<  GridPartType,
                                                        FiniteElementType,
                                                        Dune::FemLocalFunctions::NoTransformation,
                                                        Dune::FemLocalFunctions::SimpleStorage,
                                                        polOrder,
                                                        polOrder >  BaseFunctionSetMapType;
public:
  typedef Dune::FemLocalFunctions::DiscreteFunctionSpace< BaseFunctionSetMapType >  BackendType;
  typedef Mapper::FemDofWrapper< typename BackendType::MapperType >                 MapperType;
  typedef BaseFunctionSet::FemLocalfunctionsWrapper< BaseFunctionSetMapType,
              DomainFieldType, dimDomain, RangeFieldType, dimRange, dimRangeCols >  BaseFunctionSetType;
  typedef typename BaseFunctionSetType::EntityType                                  EntityType;
private:
  template< class G, int p, class R, int r, int rC >
  friend class FemLocalfunctionsWrapper;
}; // class FemLocalfunctionsWrapperTraits< ..., 1, 1 >


template< class GridPartImp, int polynomialOrder, class RangeFieldImp >
class FemLocalfunctionsWrapper< GridPartImp, polynomialOrder, RangeFieldImp, 1, 1 >
  : public SpaceInterface< FemLocalfunctionsWrapperTraits< GridPartImp, polynomialOrder, RangeFieldImp, 1, 1 > >
{
  typedef SpaceInterface< FemLocalfunctionsWrapperTraits< GridPartImp, polynomialOrder, RangeFieldImp, 1, 1 > >
    BaseType;
  typedef FemLocalfunctionsWrapper< GridPartImp, polynomialOrder, RangeFieldImp, 1, 1 > ThisType;
public:
  typedef FemLocalfunctionsWrapperTraits< GridPartImp, polynomialOrder, RangeFieldImp, 1, 1 > Traits;

  typedef typename Traits::GridPartType   GridPartType;
  typedef typename GridPartType::ctype    DomainFieldType;
  static const int                        polOrder = Traits::polOrder;
  static const unsigned int               dimDomain = GridPartType::dimension;
  typedef typename Traits::RangeFieldType RangeFieldType;
  static const unsigned int               dimRange = Traits::dimRange;
  static const unsigned int               dimRangeCols = Traits::dimRangeCols;

  typedef typename Traits::BackendType          BackendType;
  typedef typename Traits::MapperType           MapperType;
  typedef typename Traits::BaseFunctionSetType  BaseFunctionSetType;
  typedef typename Traits::EntityType           EntityType;

  typedef Dune::Stuff::LA::SparsityPatternDefault PatternType;

private:
  typedef typename Traits::BaseFunctionSetMapType BaseFunctionSetMapType;

public:
  FemLocalfunctionsWrapper(std::shared_ptr< const GridPartType > gridP)
    : gridPart_(assertGridPart(gridP))
    , baseFunctionSetMap_(new BaseFunctionSetMapType(*gridPart_))
    , backend_(new BackendType(const_cast< GridPartType& >(*gridPart_), *baseFunctionSetMap_))
    , mapper_(new MapperType(backend_->mapper()))
  {}

  FemLocalfunctionsWrapper(const ThisType& other)
    : gridPart_(other.gridPart_)
    , baseFunctionSetMap_(other.baseFunctionSetMap_)
    , backend_(other.backend_)
    , mapper_(other.mapper_)
  {}

  ThisType& operator=(const ThisType& other)
  {
    if (this != &other) {
      gridPart_ = other.gridPart_;
      baseFunctionSetMap_ = other.baseFunctionSetMap_;
      backend_ = other.backend_;
      mapper_ = other.mapper_;
    }
    return *this;
  }

  ~FemLocalfunctionsWrapper() {}

  std::shared_ptr< const GridPartType > gridPart() const
  {
    return gridPart_;
  }

  const BackendType& backend() const
  {
    return *backend_;
  }

  bool continuous() const
  {
    return false;
  }

  const MapperType& mapper() const
  {
    return *mapper_;
  }

  BaseFunctionSetType baseFunctionSet(const EntityType& entity) const
  {
    return BaseFunctionSetType(*baseFunctionSetMap_, entity);
  }

  template< class R >
  void localConstraints(const EntityType& /*entity*/,
                        Constraints::LocalDefault< R >& /*ret*/) const
  {
    dune_static_assert(Dune::AlwaysFalse< R >::value, "ERROR: not implemented for arbitrary constraints!");
  }

  using BaseType::computePattern;

  template< class LocalGridPartType, class OtherSpaceType >
  PatternType* computePattern(const LocalGridPartType& localGridPart,
                              const OtherSpaceType& otherSpace) const
  {
    return BaseType::computeCodim0AndCodim1Pattern(localGridPart, otherSpace);
  }

private:
  static std::shared_ptr< const GridPartType > assertGridPart(const std::shared_ptr< const GridPartType > gP)
  {
    // static grid check
    typedef typename GridPartType::GridType GridType;
#if HAVE_ALUGRID
    dune_static_assert(!(Dune::is_same< GridType, Dune::ALUCubeGrid< dimDomain, dimDomain > >::value),
                       "This space is only implemented for simplicial grids!");
#endif
    dune_static_assert(!(Dune::is_same< GridType, Dune::SGrid< dimDomain, dimDomain > >::value),
                       "This space is only implemented for simplicial grids!");
    dune_static_assert(!(Dune::is_same< GridType, Dune::YaspGrid< dimDomain > >::value),
                       "This space is only implemented for simplicial grids!");
    // dynamic gridpart check
    typedef typename Dune::Fem::AllGeomTypes< typename GridPartType::IndexSetType,
                                              typename GridPartType::GridType > AllGeometryTypes;
    const AllGeometryTypes allGeometryTypes(gP->indexSet());
    const std::vector< Dune::GeometryType >& geometryTypes = allGeometryTypes.geomTypes(0);
    if (!(geometryTypes.size() == 1 && geometryTypes[0].isSimplex()))
      DUNE_THROW(Dune::NotImplemented,
                 "\n" << Dune::Stuff::Common::colorStringRed("ERROR:")
                 << " this space is only implemented for simplicial grids!");
    return gP;
  } // ... assertGridPart(...)

  std::shared_ptr< const GridPartType > gridPart_;
  std::shared_ptr< BaseFunctionSetMapType > baseFunctionSetMap_;
  std::shared_ptr< const BackendType > backend_;
  std::shared_ptr< const MapperType > mapper_;
}; // class FemLocalfunctionsWrapper< ..., 1, 1 >


} // namespace DiscontinuousLagrangeSpace
} // namespace GDT
} // namespace Dune

#endif // DUNE_GDT_SPACE_DISCONTINUOUSLAGRANGE_FEM_LOCALFUNCTIONS_HH
