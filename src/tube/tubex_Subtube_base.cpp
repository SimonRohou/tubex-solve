/* ============================================================================
 *  tubex-lib - Subtube class (base methods)
 * ============================================================================
 *  Copyright : Copyright 2017 Simon Rohou
 *  License   : This program is distributed under the terms of
 *              the GNU Lesser General Public License (LGPL).
 *
 *  Author(s) : Simon Rohou
 *  Bug fixes : -
 *  Created   : 2015
 * ---------------------------------------------------------------------------- */

#include "tubex_Subtube.h"
#include "ibex.h"
#include "exceptions/tubex_Exception.h"
#include "exceptions/tubex_DomainException.h"
#include "exceptions/tubex_EmptyException.h"
#include <iostream>
#include <limits>
#include <iomanip> // for setprecision()
#ifdef _OPENMP
  #include <omp.h> // for multithreading
#endif

using namespace std;
using namespace ibex;

namespace tubex
{
  // Public methods

    // Definition

      Subtube::Subtube()
      {

      }

      Subtube::Subtube(const Subtube& x)
      {

      }
      
      Subtube::Subtube(const Interval& domain, const Interval& value)
      {

      }
      
      Subtube::Subtube(const Subtube& x, const Interval& value)
      {

      }
      
      Subtube::~Subtube()
      {

      }
      
      const Interval& Subtube::domain() const
      {

      }  

    // Slices structure

      int Subtube::nbSlices() const
      {

      }
      
      IntervalVector Subtube::sliceBox(int slice_id) const
      {

      }
      
      bool Subtube::isSlice() const
      {

      }
      
      Subtube* Subtube::getSlice(int slice_id)
      {

      }
      
      const Subtube* Subtube::getSlice(int slice_id) const
      {

      }
      
      void Subtube::getSlices(vector<Subtube*>& v_slices) const
      {

      }
      
      int Subtube::input2index(double t) const
      {

      }
      
      double Subtube::index2input(int slice_id) const
      {

      }
      
      const Interval& Subtube::sliceDomain(int slice_id) const
      {

      }
      
      const Interval& Subtube::sliceDomain(double t) const
      {

      }
      
      void Subtube::sample(int slice_id)
      {

      }

    // Access values

      double Subtube::volume() const
      {

      }
      
      double Subtube::dist(const Subtube& x) const
      {

      }
      
      const Interval& Subtube::image() const
      {

      }
      
      const Interval& Subtube::operator[](int slice_id) const
      {

      }
      
      Interval Subtube::operator[](double t) const
      {

      }
      
      Interval Subtube::operator[](const Interval& t) const
      {

      }
      
      const pair<Interval,Interval> Subtube::eval(const Interval& t) const
      {

      }
      
      Interval Subtube::interpol(double t, const Subtube& derivative) const
      {

      }
      
      Interval Subtube::interpol(const Interval& t, const Subtube& derivative) const
      {

      }
      
      pair<Interval,Interval> Subtube::partialInterpol(const Interval& t, const Subtube& derivative) const
      {

      }
      
      Interval Subtube::invert(const Interval& y, const Interval& t) const
      {

      }
      
      void Subtube::invert(const Interval& y, vector<Interval> &v_t, const Interval& t) const
      {

      }
      
      double Subtube::maxThickness() const
      {

      }
      
      double Subtube::maxThickness(int& first_id_max_thickness) const
      {

      }
      
    // Tests

      bool Subtube::isEmpty() const
      {

      }
      
      bool Subtube::operator==(const Subtube& x) const
      {

      }
      
      bool Subtube::operator!=(const Subtube& x) const
      {

      }
    
    // Setting values

      void Subtube::set(const Interval& y, int slice_id)
      {

      }
      
      void Subtube::setEmpty()
      {

      }
      
      bool Subtube::contract(const Interval& y, int slice_id)
      {

      }
      
      Subtube& Subtube::inflate(const Interval& y, int slice_id)
      {

      }
      
      Subtube& Subtube::inflate(const Interval& y, double t)
      {

      }
      
      Subtube& Subtube::inflate(double rad)
      {

      }

    // Operators

      Subtube& Subtube::operator=(const Subtube& x)
      {

      }
      
      Subtube& Subtube::operator|=(const Subtube& x)
      {

      }
      
      Subtube& Subtube::operator|=(const Trajectory& x)
      {

      }
      
      Subtube& Subtube::operator&=(const Subtube& x)
      {

      }

    // String

      ostream& operator<<(ostream& str, const Subtube& x)
      {

      }

  // Protected methods

    // Definition

      Subtube::Subtube(const vector<Interval>& v_slices_domains, const Interval& value)
      {

      }

    // Slices/tree structure

      void Subtube::createFromSlicesVector(const vector<Interval>& v_slices_domains, const Interval& value)
      {

      }
      
      const Subtube* Subtube::getFirstSubtube() const
      {

      }
          
      const Subtube* Subtube::getSecondSubtube() const
      {

      }
          
      void Subtube::getSubtubeNodes(vector<Subtube*> &v_nodes)
      {

      }
      
      void Subtube::getSubtubeNodes(vector<const Subtube*> &v_nodes) const
      {

      }
          
      void Subtube::computeTree() const
      {

      }
      
      void Subtube::flagFutureTreeComputation(int slice_id) const
      {

      }
    
    // Access values

      void Subtube::invert(const Interval& y, vector<Interval> &v_t, const Interval& search_domain, bool concatenate_results) const
      {

      }
    
    // Operators

      void Subtube::unionWith_localUpdate(const Subtube *x)
      {

      }
      
      void Subtube::intersectWith_localUpdate(const Subtube *x)
      {

      }
}