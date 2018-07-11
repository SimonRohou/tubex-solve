/* ============================================================================
 *  tubex-lib - Solver class
 * ============================================================================
 *  Copyright : Copyright 2017 Simon Rohou
 *  License   : This program is distributed under the terms of
 *              the GNU Lesser General Public License (LGPL).
 *
 *  Author(s) : Simon Rohou
 *  Bug fixes : -
 *  Created   : 2018
 * ---------------------------------------------------------------------------- */

#ifndef __TUBEX_SOLVER_H__
#define __TUBEX_SOLVER_H__

#include <vector>
#include "tubex_TubeVector.h"

namespace tubex
{
  class Solver
  {
    public:

      Solver();
      const std::vector<TubeVector> solve(const TubeVector& x0,
                                          void (*ctc_func)(TubeVector&), // todo: contractor object
                                         const ibex::Vector& max_thickness,
                                          float refining_ratio = 0.005,
                                          float fixed_point_ratio = 0.005);
      void cid(TubeVector &x, void (*ctc_func)(TubeVector&));

    protected:
      
  };
}

#endif