/* ============================================================================
 *  tubex-lib - TubeSlice class
 * ============================================================================
 *  Copyright : Copyright 2017 Simon Rohou
 *  License   : This program is distributed under the terms of
 *              the GNU Lesser General Public License (LGPL).
 *
 *  Author(s) : Simon Rohou
 *  Bug fixes : -
 *  Created   : 2015
 * ---------------------------------------------------------------------------- */

#ifndef TUBESLICE_HEADER
#define TUBESLICE_HEADER

#include "tubex_TubeNode.h"

namespace tubex
{
  class TubeSlice : public TubeNode
  {
    public:

    /** Base: **/

      // Definition
      TubeSlice(const ibex::Interval& domain, const ibex::Interval& codomain = ibex::Interval::ALL_REALS);
      TubeSlice(const TubeSlice& x);
      ~TubeSlice();
      TubeSlice& operator=(const TubeSlice& x);

      // Slices structure
      bool isSlice() const;
      TubeSlice* getSlice(int slice_id);
      const TubeSlice* getSlice(int slice_id) const;
      TubeSlice* getSlice(double t);
      const TubeSlice* getSlice(double t) const;
      void getSlices(std::vector<const TubeSlice*>& v_slices) const;
      int input2index(double t) const;
      void getTubeNodes(std::vector<const TubeNode*> &v_nodes) const;
      TubeSlice* prevSlice() const;
      TubeSlice* nextSlice() const;
      static void chainSlices(TubeSlice *first_slice, TubeSlice *second_slice);
      const ibex::Interval inputGate() const;
      const ibex::Interval outputGate() const;

      // Access values
      const ibex::Interval& codomain() const;
      double volume() const;
      const ibex::Interval operator[](double t) const;
      const ibex::Interval operator[](const ibex::Interval& search_domain) const;
      //ibex::Interval interpol(double t, const TubeSlice& derivative) const;
      ibex::Interval invert(const ibex::Interval& y, const ibex::Interval& search_domain = ibex::Interval::ALL_REALS) const;
      const std::pair<ibex::Interval,ibex::Interval> eval(const ibex::Interval& t = ibex::Interval::ALL_REALS) const;

      // Tests
      bool operator==(const TubeSlice& x) const;
      bool operator!=(const TubeSlice& x) const;
      bool isSubset(const TubeSlice& x) const;
      bool isStrictSubset(const TubeSlice& x) const;
      bool isEmpty() const;
      bool encloses(const Trajectory& x) const;

      // Setting values
      void set(const ibex::Interval& y);
      void setEmpty();
      void setInputGate(const ibex::Interval& input_gate);
      void setOutputGate(const ibex::Interval& output_gate);

      /*
      // String
      friend std::ostream& operator<<(std::ostream& str, const TubeSlice& x);*/

    protected:

    /** Base: **/

      // Access values
      void invert(const ibex::Interval& y, std::vector<ibex::Interval> &v_t, const ibex::Interval& search_domain, bool concatenate_results) const;

      // Tests
      bool isEqual(const TubeSlice& x) const;
      bool isDifferent(const TubeSlice& x) const;

    /** Class variables **/

      TubeSlice *m_prev_slice = NULL, *m_next_slice = NULL;
      ibex::Interval *m_input_gate = NULL, *m_output_gate = NULL;

      friend class TubeTree;
  };
}

#endif