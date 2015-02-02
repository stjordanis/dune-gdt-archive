// This file is part of the dune-gdt project:
//   http://users.dune-project.org/projects/dune-gdt
// Copyright holders: Felix Schindler
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)

#ifndef DUNE_GDT_PLAYGROUND_SPACES_DG_FEM_LOCALFUNCTIONS_HH
#define DUNE_GDT_PLAYGROUND_SPACES_DG_FEM_LOCALFUNCTIONS_HH

#include <dune/common/deprecated.hh>

#include <dune/geometry/genericgeometry/topologytypes.hh>

#include <dune/grid/common/capabilities.hh>

#if HAVE_DUNE_FEM_LOCALFUNCTIONS
# include <dune/localfunctions/lagrange/equidistantpoints.hh>
# include <dune/localfunctions/lagrange.hh>

# include <dune/fem_localfunctions/localfunctions/transformations.hh>
# include <dune/fem_localfunctions/basefunctions/genericbasefunctionsetstorage.hh>
# include <dune/stuff/common/disable_warnings.hh>
#   include <dune/fem_localfunctions/basefunctionsetmap/basefunctionsetmap.hh>
# include <dune/stuff/common/reenable_warnings.hh>
# include <dune/fem_localfunctions/space/genericdiscretefunctionspace.hh>
#endif // HAVE_DUNE_FEM_LOCALFUNCTIONS

#include <dune/stuff/common/color.hh>
#include <dune/stuff/common/type_utils.hh>

#include <dune/gdt/mapper/fem.hh>
#include <dune/gdt/basefunctionset/fem-localfunctions.hh>
#include <dune/gdt/spaces/constraints.hh>
#include <dune/gdt/spaces/interface.hh>

namespace Dune {
namespace GDT {
namespace Spaces {
namespace DG {

#if HAVE_DUNE_FEM_LOCALFUNCTIONS


// forward, to be used in the traits and to allow for specialization
template< class GridPartImp, int polynomialOrder, class RangeFieldImp, int rangeDim, int rangeDimCols = 1 >
class FemLocalfunctionsBased
{
  static_assert(rangeDim == 1 && rangeDimCols == 1, "Not yet implemented (find suitable vector valued basis)!");
  static_assert(Dune::AlwaysFalse< GridPartImp >::value, "Untested for these dimensions!");
};


template< class GridPartImp, int polynomialOrder, class RangeFieldImp, int rangeDim, int rangeDimCols >
class FemLocalfunctionsBasedTraits
{
  static_assert(polynomialOrder >= 1, "Wrong polOrder given!");
  static_assert(rangeDim == 1, "Not yet implemented (find suitable vector valued basis)!");
  static_assert(rangeDimCols == 1, "Not yet implemented (find suitable vector valued basis)!");
public:
  typedef GridPartImp                   GridPartType;
  typedef typename GridPartType::GridViewType GridViewType;
  static const int                      polOrder = polynomialOrder;
private:
  typedef typename GridPartType::ctype  DomainFieldType;
  static const unsigned int             dimDomain = GridPartType::dimension;
  typedef typename GridPartType::GridType GridType;
  static_assert(dimDomain == 1 || Dune::Capabilities::hasSingleGeometryType< GridType >::v,
                "This space is only implemented for fully simplicial grids!");
  static_assert(dimDomain == 1 || (Dune::Capabilities::hasSingleGeometryType< GridType >::topologyId
                                   == GenericGeometry::SimplexTopology< dimDomain >::type::id),
                "This space is only implemented for fully simplicial grids!");
public:
  typedef RangeFieldImp                 RangeFieldType;
  typedef FemLocalfunctionsBased< GridPartType, polOrder, RangeFieldType, rangeDim, rangeDimCols > derived_type;
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
              DomainFieldType, dimDomain, RangeFieldType, rangeDim, rangeDimCols >  BaseFunctionSetType;
  typedef typename BaseFunctionSetType::EntityType                                  EntityType;
  static const Stuff::Grid::ChoosePartView part_view_type = Stuff::Grid::ChoosePartView::part;
  static const bool needs_grid_view = false;
  typedef double CommunicatorType;
private:
  template< class G, int p, class R, int r, int rC >
  friend class FemLocalfunctionsBased;
}; // class FemLocalfunctionsBasedTraits


template< class GridPartImp, int polynomialOrder, class RangeFieldImp >
class FemLocalfunctionsBased< GridPartImp, polynomialOrder, RangeFieldImp, 1, 1 >
  : public SpaceInterface< FemLocalfunctionsBasedTraits< GridPartImp, polynomialOrder, RangeFieldImp, 1, 1 >,
                           typename GridPartImp::dimension, 1, 1 >
{
  typedef SpaceInterface< FemLocalfunctionsBasedTraits< GridPartImp, polynomialOrder, RangeFieldImp, 1, 1 >,
                          typename GridPartImp::dimension, 1, 1 >
    BaseType;
  typedef FemLocalfunctionsBased< GridPartImp, polynomialOrder, RangeFieldImp, 1, 1 > ThisType;
public:
  typedef FemLocalfunctionsBasedTraits< GridPartImp, polynomialOrder, RangeFieldImp, 1, 1 > Traits;

  typedef typename Traits::GridPartType   GridPartType;
  typedef typename Traits::GridViewType   GridViewType;
  typedef typename GridPartType::ctype    DomainFieldType;
  static const int                        polOrder = Traits::polOrder;
  static const unsigned int               dimDomain = BaseType::dimDomain;
private:
  static_assert(GridPartType::dimension == dimDomain, "Dimension of GridPart has to match dimDomain");
public:
  typedef typename Traits::RangeFieldType RangeFieldType;
  static const unsigned int               dimRange = BaseType::dimRange;
  static const unsigned int               dimRangeCols = BaseType::dimRangeCols;

  typedef typename Traits::BackendType          BackendType;
  typedef typename Traits::MapperType           MapperType;
  typedef typename Traits::BaseFunctionSetType  BaseFunctionSetType;
  typedef typename Traits::EntityType           EntityType;

  typedef Dune::Stuff::LA::SparsityPatternDefault PatternType;

private:
  typedef typename Traits::BaseFunctionSetMapType BaseFunctionSetMapType;

public:
  FemLocalfunctionsBased(GridPartType gridP)
    : gridPart_(gridP)
    , gridView_(gridPart_->gridView())
    , baseFunctionSetMap_(gridPart_)
    , backend_(const_cast< GridPartType& >(gridPart_), baseFunctionSetMap_)
    , mapper_(backend_->mapper())
    , communicator_(0.0)
  {}

  FemLocalfunctionsBased(const ThisType& other) = default;

  ThisType& operator=(const ThisType& other)
  {
    if (this != &other) {
      gridPart_ = other.gridPart_;
      gridView_ = other.gridView_;
      baseFunctionSetMap_ = other.baseFunctionSetMap_;
      backend_ = other.backend_;
      mapper_ = other.mapper_;
    }
    return *this;
  }

  const GridPartType& grid_part() const
  {
    return gridPart_;
  }

  const GridViewType& grid_view() const
  {
    return gridView_;
  }

  const BackendType& backend() const
  {
    return backend_;
  }

  const MapperType& mapper() const
  {
    return mapper_;
  }

  BaseFunctionSetType base_function_set(const EntityType& entity) const
  {
    return BaseFunctionSetType(baseFunctionSetMap_, entity);
  }

  template< class R >
  void local_constraints(const EntityType& /*entity*/, Constraints::LocalDefault< R >& /*ret*/) const
  {
    static_assert(Dune::AlwaysFalse< R >::value, "Not implemented for arbitrary constraints!");
  }

  using BaseType::compute_pattern;

  template< class G, class S >
  PatternType compute_pattern(const GridView< G >& local_grid_view, const SpaceInterface< S >& ansatz_space) const
  {
    return BaseType::compute_face_and_volume_pattern(local_grid_view, ansatz_space);
  }

  double& communicator() const
  {
    return communicator_;
  }

private:
  const GridPartType gridPart_;
  const GridViewType gridView_;
  BaseFunctionSetMapType  baseFunctionSetMap_;
  const BackendType backend_;
  const MapperType mapper_;
  mutable double communicator_;
}; // class FemLocalfunctionsBased< ..., 1, 1 >


#else // HAVE_DUNE_FEM_LOCALFUNCTIONS


template< class GridPartImp, int polynomialOrder, class RangeFieldImp, int rangeDim, int rangeDimCols = 1 >
class FemLocalfunctionsBased
{
  static_assert(Dune::AlwaysFalse< GridPartImp >::value, "You are missing dune-fem-localfunctions!");
};


#endif // HAVE_DUNE_FEM_LOCALFUNCTIONS


} // namespace DG
} // namespace Spaces
} // namespace GDT
} // namespace Dune

#endif // DUNE_GDT_PLAYGROUND_SPACES_DG_FEM_LOCALFUNCTIONS_HH