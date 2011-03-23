
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

// system includes
#include <iostream>

// disable warnings about problems in dune headers
#include <dune/fem-tools/header/disablewarnings.hh>

// dune common includes
#include <dune/common/exceptions.hh>
#include <dune/common/fvector.hh>

// dune grid includes
#include <dune/grid/yaspgrid.hh>
#include <dune/grid/io/file/dgfparser/dgfparser.hh>

// dune fem includes
#include <dune/fem/misc/mpimanager.hh>
#include <dune/fem/gridpart/adaptiveleafgridpart.hh>
#include <dune/fem/space/common/functionspace.hh>
#include <dune/fem/function/common/function.hh>
#include <dune/fem/space/lagrangespace.hh>

// reenable warnings
#include <dune/fem-tools/header/enablewarnings.hh>

// dune fem-functionals includes
#include <dune/fem/common/localbasefunction.hh>

// disable warnings about problems in dune headers (sourced by dgfparser)
#include <dune/fem-tools/header/disablewarnings.hh>

// main
int main(int argc, char** argv)
{
  try{

    // print welcome
    std::cout << "Local basefunction test ";

    // mpi
    Dune::MPIManager::initialize ( argc, argv );

    // dimension and grid
    const int dim = 2;

    typedef Dune::YaspGrid< dim >
      GridType;

    Dune::GridPtr< GridType > gridPtr( "unitcube_2d.dgf" );

    typedef Dune::AdaptiveLeafGridPart< GridType >
      GridPartType;

    GridPartType gridPart( *gridPtr );

    // analytical function space
    typedef Dune::FunctionSpace< double, double, dim, 1 >
      AnalyticalFunctionSpaceType;

    // discrete function space
    const int polOrder = 1;

    typedef Dune::LagrangeDiscreteFunctionSpace< AnalyticalFunctionSpaceType, GridPartType, polOrder >
      DiscreteFunctionSpaceType;

    const DiscreteFunctionSpaceType discreteFunctionSpace( gridPart );

    // local basefunctionset provider
    typedef Dune::Functionals::Common::LocalBaseFunctionProvider< DiscreteFunctionSpaceType >
      LocalBaseFunctionProviderType;

    LocalBaseFunctionProviderType localBaseFunctionProvider( discreteFunctionSpace );

    // entity
    LocalBaseFunctionProviderType::EntityIteratorType entityIterator = discreteFunctionSpace.begin();
    LocalBaseFunctionProviderType::EntityType& entity = *entityIterator;

    // local basefunction
    typedef LocalBaseFunctionProviderType::LocalBaseFunctionType
      LocalBaseFunctionType;

    LocalBaseFunctionType localBaseFunction = localBaseFunctionProvider.provide( entity, 0 );

    // test some functionality


    // if we get here, the test passed
    std::cout << "passed!" << std::endl;

    // we don't make no errors^^
    return 0;
  }
  catch (Dune::Exception &e){
    std::cerr << "Dune reported error: " << e << std::endl;
  }
  catch (...){
    std::cerr << "Unknown exception thrown!" << std::endl;
  }
} // end main