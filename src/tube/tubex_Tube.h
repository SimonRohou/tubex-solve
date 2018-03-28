/* ============================================================================
 *  tubex-lib - Tube class
 * ============================================================================
 *  Copyright : Copyright 2017 Simon Rohou
 *  License   : This program is distributed under the terms of
 *              the GNU Lesser General Public License (LGPL).
 *
 *  Author(s) : Simon Rohou
 *  Bug fixes : -
 *  Created   : 2015
 * ---------------------------------------------------------------------------- */

#ifndef TUBE_HEADER
#define TUBE_HEADER

#include "tubex_TubeNode.h"

namespace tubex
{
  class Tube : public TubeNode
  {
    public:

      Tube(const ibex::Interval& domain);
      ibex::Interval operator()(double t) const;

    protected:

    /** Class variables **/

      TubeNode *m_first_tubenode = NULL, *m_second_tubenode = NULL;
  };
}

#endif