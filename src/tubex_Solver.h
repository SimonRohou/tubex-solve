/* ============================================================================
 *  tubex-lib - Solver class
 * ============================================================================
 *  Copyright : Copyright 2017 Simon Rohou
 *  License   : This program is distributed under the terms of
 *              the GNU Lesser General Public License (LGPL).
 *
 *  Author(s) : Simon Rohou, Bertrand Neveu
 *  Bug fixes : -
 *  Created   : 2018
 * ---------------------------------------------------------------------------- */

#ifndef __TUBEX_SOLVER_H__
#define __TUBEX_SOLVER_H__

#include <list>
#include <vector>

#include "tubex_TubeVector.h"
#include "tubex_TrajectoryVector.h"
#include "tubex_VIBesFigTubeVector.h"
#include "tubex_CtcPicard.h"

#include "tubex_CtcDeriv.h"
#include "tubex_CtcIntegration.h"
#include "tubex_CtcDynCid.h"
#include "tubex_CtcDynCidGuess.h"
#include "tubex_CtcDynBasic.h"

using namespace std;
namespace tubex
{
  class Solver
  {
  public:

      Solver(const ibex::Vector& max_thickness);
      ~Solver();

      // Ratios used for stopping fixed point algorithms 
      void set_refining_fxpt_ratio(float refining_fxpt_ratio);
      void set_propa_fxpt_ratio(float propa_fxpt_ratio);
      void set_var3b_fxpt_ratio(float var3b_fxpt_ratio);
      void set_var3b_propa_fxpt_ratio(float propa_fxpt_ratio);

      // Slice choice for var3b
      void set_var3b_timept(int var3b_timept); // where do the var3b contraction : 1 domain.ub(); -1 domain.lb(); 0 max_diam_gate(); 2 randomly domain.ub() or domain.lb()

      // Slice choice for bisection
      void set_bisection_timept (int bisection_timept); // where to bisect : 1 domain.ub(); -1 domain.lb(); 0 max_diam_gate(); 2 randomly domain.ub() or domain.lb(); 3 round robin  domain.ub() and domain.lb() ; -2 no bisection

      // no more refining when  max_slices is  reached
      void set_max_slices(int max_slices); 

      // refining mode : which slices to refine
      void set_refining_mode(int refining_mode); // 0 : all slices ; 1 : one slice ; 2 : the slices with a difference between input and output gates greater  than average ; 3 : the slices with with a difference between input and output gates greater  than the median.
 
      // contraction mode :
      void set_contraction_mode(int contraction_mode) ; // 0 for CtcDynBasic, 1 for CtcDynCid, 2 for CtcDynCidGuess, 4 for CtcDeriv
      void set_trace(int trace);
      double solving_time;

      /* the solve method, it as for parameters a tube vector x0 , and 3 possibilities
         -  a tube vector contractor ctc_func (for general problems as Integrodifferential problems)
         - a differential function (TFnc computing the derivative of a tube vector) (for pure ODEs)
         - a differential function and a tube vector contractor (for ODE problems with side constraints)

         The returned results are tubes containing the solutions. Two tubes have at least a disjoint gate.
      */


      const std::list<TubeVector> solve(const TubeVector& x0, void (*ctc_func)(TubeVector&));
      const std::list<TubeVector> solve(const TubeVector& x0, TFnc & f,void (*ctc_func)(TubeVector&)=NULL );
      const std::list<TubeVector> solve(const TubeVector& x0, TFnc* f,void (*ctc_func)(TubeVector&));

      VIBesFigTubeVector* figure();
      static const ibex::BoolInterval solutions_contain(const std::list<TubeVector>& l_solutions, const TrajectoryVector& truth);
      //      void (*ctc_func) (TubeVector&);

  protected:
      double one_finite_gate(TubeVector &x);
      bool empty_intersection(TubeVector& t1, TubeVector& t2);
      void clustering(std::list<std::pair<int,TubeVector> >& l_tubes);
      void clustering(std::list<TubeVector>& l_tubes);
      bool stopping_condition_met(const TubeVector& x);
      bool fixed_point_reached(double volume_before, double volume_after, float fxpt_ratio);
      void propagation(TubeVector &x, TFnc* f, void (*ctc_func)(TubeVector&), float propa_fxpt_ratio, bool incremental, double t0);
      void deriv_contraction (TubeVector &x, TFnc& f);
      void integration_contraction(TubeVector &x, TFnc& f, double t0, bool incremental);
      void picard_contraction (TubeVector &x, TFnc& f);
      void var3b(TubeVector &x,TFnc* f, void (*ctc_func)(TubeVector&));
      bool refining (TubeVector &x);
      double average_refining_threshold(const TubeVector &x, 
				vector<double>& slice_step, vector<double>& t_refining);
      double median_refining_threshold(const TubeVector &x, 
				vector<double>& slice_step, vector<double>& t_refining);
      bool refining_with_threshold(TubeVector & v, int nb_slices);


      void bisection_guess (TubeVector & x, TFnc& f);
      std::pair<int,std::pair<double,double>> bisection_guess(TubeVector x, TubeVector v, DynCtc* slice_ctr, TFnc& fnc, int variant);
      std::pair<int,std::pair<double,double>> bisection_guess(TubeVector x, TubeVector v, DynCtc* slice_ctr, TFnc& fnc);

      ibex::Vector m_max_thickness = ibex::Vector(1);
      float m_refining_fxpt_ratio = 0.005;
      float m_propa_fxpt_ratio = 0.0 ;
      float m_var3b_fxpt_ratio = 0.0; // one call no fix point
      float m_var3b_propa_fxpt_ratio = 0.0;
      /* Internal parameters for var3b algorithm */
      float m_var3b_bisection_minrate = 0.0001;
      float m_var3b_bisection_maxrate = 0.4;
      int m_var3b_bisection_ratefactor=2;

      int m_var3b_timept=0;
      int m_bisection_timept=0;
      int m_trace=0;
      int m_max_slices=5000;
      int m_refining_mode=0;
      int m_contraction_mode=0; 
      // Embedded graphics
      VIBesFigTubeVector *m_fig = NULL;
      int bisections=0;
  };
}

#endif
