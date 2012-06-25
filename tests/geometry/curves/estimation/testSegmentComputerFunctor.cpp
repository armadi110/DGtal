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
 * @file testSegmentComputerFunctor.cpp
 * @author Tristan Roussillon (\c
 * tristan.roussillon@liris.cnrs.fr ) Laboratoire d'InfoRmatique en
 * Image et Systèmes d'information - LIRIS (CNRS, UMR 5205), CNRS,
 * France
 *
 *
 * @date 2011/06/28

 * Functions for testing classes in SegmentComputerFunctor.h
 *
 * This file is part of the DGtal library.
 */

///////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <cmath>
#include "DGtal/base/Common.h"


#include "DGtal/geometry/curves/ArithmeticalDSS.h"
#include "DGtal/geometry/curves/GeometricalDCA.h"
#include "DGtal/geometry/curves/estimation/SegmentComputerFunctor.h"

#include "DGtal/io/boards/Board2D.h"


#include "ConfigTest.h"

///////////////////////////////////////////////////////////////////////////////

using namespace std;
using namespace DGtal;
using namespace LibBoard;

///////////////////////////////////////////////////////////////////////////////
// Functions for testing classes in SegmentComputerFunctor.h.
///////////////////////////////////////////////////////////////////////////////


/**
 * TangentFromDSSFunctor
 *
 */
template<typename DSSComputer>
bool testTangentFromDSS(
     const typename DSSComputer::ConstIterator& itb,
     const typename DSSComputer::ConstIterator& ite  )
{


  trace.info() << "feeding segment computer " << endl;

  DSSComputer dss;  
  DGtal::longestSegment( dss, itb, ite ); 

  trace.info() << dss << endl;
  trace.info() << endl;

  trace.info() << "building and using the functor " << endl;
  const double epsilon = 0.00001;

  int nb = 0; 
  int nbok = 0; 

  { //angle
    TangentAngleFromDSSFunctor<DSSComputer> f; 
    double q1 = f(dss); 
    double q2 = std::atan2((double)dss.getA(),(double)dss.getB());
    trace.info() << "Tangent orientation : " << q1 << " == " << q2 << endl;
    nbok += (std::abs(q1 - q2) < epsilon)?1:0;
    nb++; 
    trace.info() << "(" << nbok << "/" << nb << ")" << std::endl; 
  }

  { //vector
    TangentVectorFromDSSFunctor<DSSComputer> f; 
    typedef typename TangentVectorFromDSSFunctor<DSSComputer>::Quantity Quantity; 
    Quantity q1 = f(dss); 
    Quantity q2 = Quantity(dss.getB(), dss.getA()); 
    trace.info() << "Tangent vector : " << q1 << " == " << q2 << endl;
    nbok += (q1 == q2)?1:0; 
    nb++; 
    trace.info() << "(" << nbok << "/" << nb << ")" << std::endl; 
  }

  { //unit vector
    TangentFromDSSFunctor<DSSComputer> f; 
    typedef typename TangentFromDSSFunctor<DSSComputer>::Quantity Quantity; 
    Quantity q1 = f(dss);
    double n = std::sqrt( (double)dss.getA()*dss.getA() + (double)dss.getB()*dss.getB() );
    Quantity q2 = Quantity((double)dss.getB()/n, (double)dss.getA()/n); 
    trace.info() << "Normalized tangent vector : " << q1 << " == " << q2 << endl;
    nbok += ((std::abs(q1[0] - q2[0]) < epsilon)&&(std::abs(q1[1] - q2[1]) < epsilon))?1:0; 
    nb++; 
    trace.info() << "(" << nbok << "/" << nb << ")" << std::endl; 
  }

  return (nb == nbok); 
}

/**
 * Estimations from DCA
 *
 */
template<typename DCAComputer>
bool testFromDCA(
     const typename DCAComputer::ConstIterator& itb,
     const typename DCAComputer::ConstIterator& ite  )
{


  trace.info() << "feeding segment computer " << endl;

  DCAComputer dca;  
  DGtal::longestSegment( dca, itb, ite ); 

  trace.info() << dca << endl;
  trace.info() << endl;

  trace.info() << "building and using the functor " << endl;
  const double epsilon = 0.00001;

  int nb = 0; 
  int nbok = 0; 

  { //curvature
    CurvatureFromDCAFunctor<DCAComputer> f; 
    f.init(1.0); 
    double q1 = f(dca);
    f.init(0.1); 
    double q2 = f(dca);    
    trace.info() << "Curvature (h=1): " << q1 << std::endl; 
    trace.info() << "Curvature (h=0.1): " << q2 << std::endl; 
    nbok += ((q1 >= 0)&&(q1 < 1)&&(q2 >= 0)&&(q2 < 10)
	     &&(std::abs(q2-(10*q1)) < epsilon))?1:0; 
    nb++;
    trace.info() << "(" << nbok << "/" << nb << ")" << std::endl; 
  }

  { //tangent
    TangentFromDCAFunctor<DCAComputer> f; 
    typedef typename TangentFromDCAFunctor<DCAComputer>::Quantity Quantity; 
    typename DCAComputer::ConstIterator it = DGtal::getMiddleIterator(itb, ite); 
    Quantity q1 = f( it, dca ); 
    trace.info() << "Tangent: " << q1 << " == [PointVector] {1, 0} " << std::endl; 
    nbok += (std::abs(q1[0]-1.0) < epsilon)
      && (std::abs(q1[1]) < epsilon) ? 1 : 0; 
    nb++;
    trace.info() << "(" << nbok << "/" << nb << ")" << std::endl; 
  }

  return (nb == nbok); 
}


///////////////////////////////////////////////////////////////////////////////
// Standard services - public :

int main( int argc, char** argv )
{
  trace.beginBlock ( "Testing segment computer functors" );
  trace.info() << "Args:";
  for ( int i = 0; i < argc; ++i )
    trace.info() << " " << argv[ i ];
  trace.info() << endl;

  bool res = true; 

  //types
  typedef PointVector<2,int> Point;


  //------------------------------------------ DSS
  {
    typedef std::vector<Point> Range;
    typedef Range::iterator ConstIterator;
    typedef ArithmeticalDSS<ConstIterator,int,4> DSS4;  
    typedef ArithmeticalDSS<ConstIterator,int,8> DSS8;  

    //input points
    Range curve4;
    curve4.push_back(Point(0,0));
    curve4.push_back(Point(1,0));
    curve4.push_back(Point(1,1));
    curve4.push_back(Point(2,1));
    curve4.push_back(Point(3,1));
    curve4.push_back(Point(3,2));
    curve4.push_back(Point(4,2));
    curve4.push_back(Point(5,2));
    curve4.push_back(Point(6,2));
    curve4.push_back(Point(6,3));
    curve4.push_back(Point(7,3));

    Range curve8;
    curve8.push_back(Point(0,0));
    curve8.push_back(Point(1,1));
    curve8.push_back(Point(2,1));
    curve8.push_back(Point(3,2));
    curve8.push_back(Point(4,2));
    curve8.push_back(Point(5,2));
    curve8.push_back(Point(6,3));
    curve8.push_back(Point(7,3));
    curve8.push_back(Point(8,4));
    curve8.push_back(Point(9,4));
    curve8.push_back(Point(10,5));

    //tests
    res = res && testTangentFromDSS<DSS4>(curve4.begin(), curve4.end())
      && testTangentFromDSS<DSS8>(curve8.begin(), curve8.end());
  }
  //------------------------------------------ DCA
  {
    typedef std::pair<Point,Point> Pair; 
    typedef std::vector<Pair> Range;
    typedef Range::const_iterator ConstIterator; 
    typedef GeometricalDCA<ConstIterator> DCA;  

    Range curve; 
    curve.push_back(std::make_pair(Point(0,0),Point(0,1))); 
    curve.push_back(std::make_pair(Point(1,0),Point(1,1))); 

    curve.push_back(std::make_pair(Point(2,1),Point(2,2))); 
    curve.push_back(std::make_pair(Point(3,1),Point(3,2))); 
    curve.push_back(std::make_pair(Point(4,1),Point(4,2))); 

    curve.push_back(std::make_pair(Point(5,2),Point(5,3))); 
    curve.push_back(std::make_pair(Point(6,2),Point(6,3))); 
    curve.push_back(std::make_pair(Point(7,2),Point(7,3))); 
    curve.push_back(std::make_pair(Point(8,2),Point(8,3))); 
    curve.push_back(std::make_pair(Point(9,2),Point(9,3))); 

    curve.push_back(std::make_pair(Point(10,1),Point(10,2))); 
    curve.push_back(std::make_pair(Point(11,1),Point(11,2))); 
    curve.push_back(std::make_pair(Point(12,1),Point(12,2))); 

    curve.push_back(std::make_pair(Point(13,0),Point(13,1))); 
    curve.push_back(std::make_pair(Point(14,0),Point(14,1))); 

    res = res && testFromDCA<DCA>(curve.begin(), curve.end())
      && testFromDCA<DCA>(curve.begin()+2, curve.begin()+12);
  }

  //----------------------------------------------------------
  trace.emphase() << ( res ? "Passed." : "Error." ) << endl;
  trace.endBlock();
  
  return res ? 0 : 1;
}
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
