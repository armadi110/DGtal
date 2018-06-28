/**
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 **/

/**
 * @file
 * @ingroup Examples
 * @author Jacques-Olivier Lachaud (\c jacques-olivier.lachaud@univ-savoie.fr )
 * Laboratory of Mathematics (CNRS, UMR 5127), University of Savoie, France
 *
 * @date 2018/06/26
 *
 * An example file named shortcuts.
 *
 * This file is part of the DGtal library.
 */

///////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include "ConfigExamples.h"
#include "DGtal/helpers/StdDefs.h"
#include "DGtal/helpers/Shortcuts.h"
#include "DGtal/base/Common.h"
///////////////////////////////////////////////////////////////////////////////

using namespace std;
using namespace DGtal;

///////////////////////////////////////////////////////////////////////////////

int main( int argc, char** argv )
{
  // 3d tests
  {
    typedef Shortcuts< Z3i::KSpace > SH3;
    trace.beginBlock ( "Setting parameters" );
    auto params = SH3::defaultParameters();
    // Set your own parameters with operator().
    params( "polynomial", "3*x^2+2*y^2+z^2-90" )
      ( "gridstep", 0.5 )
      ( "noise",    0.2 )
      ( "surfaceComponents", "All" );
    std::cout << params << std::endl;
    trace.endBlock();
    trace.beginBlock ( "Making implicit shape" );
    auto implicit_shape = SH3::makeImplicitShape3D( params );
    std::cout << *implicit_shape << std::endl;
    trace.endBlock();
    trace.beginBlock ( "Making Khalimsky space" );
    auto K = SH3::getKSpace( params );
    std::cout << K << std::endl;
    trace.endBlock();
    trace.beginBlock ( "Making implicit digital shape" );
    auto digital_shape = SH3::makeDigitizedImplicitShape3D( implicit_shape, params );
    std::cout << *digital_shape << std::endl;
    trace.endBlock();
    trace.beginBlock ( "Making binary image from implicit digital shape" );
    auto binary_image = SH3::makeBinaryImage( digital_shape, params );
    std::cout << *binary_image << std::endl;
    trace.endBlock();
    trace.beginBlock ( "Save binary image into file" );
    auto ok = SH3::saveBinaryImage( binary_image, "dummy.vol" );
    std::cout << ( ok ? "dummy.vol OK" : "dummy.vol ERROR" ) << std::endl;
    trace.endBlock();
    trace.beginBlock ( "Making binary image from vol file" );
    auto al_capone = SH3::makeBinaryImage( examplesPath + "samples/Al.100.vol", params );
    std::cout << *al_capone << std::endl;
    auto ok2 = SH3::saveBinaryImage( al_capone, "dummy2.vol" );
    std::cout << ( ok ? "dummy2.vol OK" : "dummy2.vol ERROR" ) << std::endl;
    trace.endBlock();
    trace.beginBlock ( "Making simple digital surface" );
    auto Kal         = SH3::getKSpace( al_capone, params );
    auto simple_surf = SH3::makeAnyBigSimpleDigitalSurface( al_capone, Kal, params );
    std::cout << "#surfels = " << simple_surf->size() << std::endl;
    std::vector< std::string > traversals { "Default", "DepthFirst", "BreadthFirst" };
    for ( auto&& mode : traversals ) {
      auto surfels = SH3::getSurfelRange( simple_surf, params( "surfaceTraversal", mode ) );
      double distance  = 0.0;
      for ( int i = 1; i < surfels.size(); ++i )
	distance += ( K.sCoords( surfels[ i-1 ] ) - K.sCoords( surfels[ i ] ) ).norm();
      std::cout << "avg " << mode << " distance = " << distance / (surfels.size()-1.0) << std::endl;
    }
    trace.endBlock();
    trace.beginBlock ( "Making all simple digital surfaces" );
    auto vec_surfs   = SH3::makeSimpleDigitalSurfaces( al_capone, Kal, params );
    std::cout << "#connected components        = " << vec_surfs.size() << std::endl;
    unsigned int nb_small = 0;
    unsigned int nb_big = 0;
    for ( auto&& surf : vec_surfs )
      {
	unsigned int n = surf->size();
	nb_small += n <  100 ? 1 : 0;
	nb_big   += n >= 100 ? 1 : 0;
      }
    std::cout << "#connected components <  100 = " << nb_small << std::endl;
    std::cout << "#connected components >= 100 = " << nb_big << std::endl;
    trace.endBlock();
    trace.beginBlock ( "Making indexed digital surface" );
    auto idx_surf    = SH3::makeIdxDigitalSurface
      ( al_capone, Kal, params( "surfaceComponents", "AnyBig" ) );
    trace.endBlock();
    trace.beginBlock ( "Traversing indexed digital surface" );
    auto positions   = idx_surf->positions();
    std::cout << "#surfels = " << idx_surf->size() << std::endl;
    for ( auto&& mode : traversals ) {
      auto surfels = SH3::getIdxSurfelRange( idx_surf, params( "surfaceTraversal", mode ) );
      double distance  = 0.0;
      for ( int i = 1; i < surfels.size(); ++i ) 
	distance += ( positions[ surfels[ i-1 ] ] - positions[ surfels[ i ] ] ).norm();
      std::cout << "avg " << mode << " distance = " << distance / (surfels.size()-1.0) << std::endl;
    }
    trace.endBlock();

    trace.beginBlock ( "Compute true normals" );
    {
      auto K       = SH3::getKSpace( params );
      auto surface = SH3::makeAnyBigSimpleDigitalSurface( binary_image, K, params );
      auto surfels = SH3::getSurfelRange( surface, params );
      auto normals = SH3::getTrueNormals( implicit_shape, K, surfels, params ); 
      std::cout << "#normals = " << normals.size() << std::endl;
    }
    trace.endBlock();

  }
  // 2d tests
  {
    typedef Shortcuts< Z2i::KSpace > SH2;
    auto params = SH2::defaultParameters();
    trace.beginBlock ( "Load and threshold gray-scale image" );
    auto gl_image = SH2::makeGrayScaleImage( examplesPath + "samples/contourS.pgm" );
    auto b_image  = SH2::makeBinaryImage( gl_image, params( "thresholdMin", 128 ) );
    auto ok       = SH2::saveBinaryImage( b_image, "dummy3.pgm" );
    std::cout << *gl_image << std::endl;
    trace.endBlock();
  } 
  return 0;
}
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
