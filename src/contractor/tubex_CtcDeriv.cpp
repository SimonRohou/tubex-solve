/* ============================================================================
 *  tubex-lib - CtcDeriv class
 * ============================================================================
 *  Copyright : Copyright 2017 Simon Rohou
 *  License   : This program is distributed under the terms of
 *              the GNU Lesser General Public License (LGPL).
 *
 *  Author(s) : Simon Rohou
 *  Bug fixes : -
 *  Created   : 2015
 * ---------------------------------------------------------------------------- */

#include "tubex_CtcDeriv.h"
#include "tubex_StructureException.h"
#include "tubex_EmptyException.h"
#include "tubex_DomainException.h"
#include "tubex_ConvexPolygon.h"

using namespace std;
using namespace ibex;

namespace tubex
{
  CtcDeriv::CtcDeriv()
  {

  }

  bool CtcDeriv::contract(TubeSlice& x, const TubeSlice& v)
  {
    StructureException::check(x, v);

    bool ctc = false;
    Interval t = x.domain();
    Interval y = x.codomain();

    ctc |= contractGates(x, v);
    ctc |= contractEnvelope(x, v, t, y);
    x.setEnvelope(y);

    return ctc;
  }

  bool CtcDeriv::contract(const TubeSlice& x, const TubeSlice& v, Interval& t, Interval& y)
  {
    StructureException::check(x, v);
    TubeSlice x_temp = x;
    contractGates(x_temp, v);
    return contractEnvelope(x_temp, v, t, y);
  }

  bool CtcDeriv::contract(Tube& x, const Tube& v)
  {
    bool ctc = false;
    ctc |= contractFwd(x, v);
    ctc |= contractBwd(x, v);
    // todo: optimized propagation, according to gates' updates
    return ctc;
  }

  bool CtcDeriv::contractFwd(Tube& x, const Tube& v) // temporal forward
  {
    bool ctc = false;
    TubeSlice *x_slice = x.getFirstSlice();
    TubeSlice *v_slice = v.getFirstSlice();

    while(x_slice != NULL)
    {
      ctc |= contract(*x_slice, *v_slice);
      x_slice = x_slice->nextSlice();
      v_slice = v_slice->nextSlice();
    }

    return ctc;
  }

  bool CtcDeriv::contractBwd(Tube& x, const Tube& v) // temporal backward
  {
    bool ctc = false;
    TubeSlice *x_slice = x.getLastSlice();
    TubeSlice *v_slice = v.getLastSlice();
    
    while(x_slice != NULL)
    {
      ctc |= contract(*x_slice, *v_slice);
      x_slice = x_slice->prevSlice();
      v_slice = v_slice->prevSlice();
    }

    return ctc;
  }

  bool CtcDeriv::contract(const Tube& x, const Tube& v, Interval& t, Interval& y)
  {
    Interval prev_y = y;
    Interval prev_t = t;

    t &= x.domain();

    TubeSlice *x_slice = (TubeSlice*)x.getSlice(t.lb());
    TubeSlice *x_last_slice = (TubeSlice*)x.getSlice(t.ub());
    TubeSlice *v_slice = (TubeSlice*)v.getSlice(t.lb());

    Interval y_union = Interval::EMPTY_SET;
    Interval t_union = Interval::EMPTY_SET;
    
    while(x_slice != x_last_slice->nextSlice())
    {
      Interval y_slice = y, t_slice = t & x_slice->domain();
      contract(*x_slice, *v_slice, t_slice, y_slice);
      y_union |= y_slice;
      t_union |= t_slice;

      x_slice = x_slice->nextSlice();
      v_slice = v_slice->nextSlice();
    }

    y &= y_union;
    t &= t_union;
    return prev_y != y || prev_t != t;
  }

  bool CtcDeriv::contractGates(TubeSlice& x, const TubeSlice& v)
  {
    bool ctc = false;
    Interval in_gate = x.inputGate(), out_gate = x.outputGate();

    // Propagations from gate to gate

    Interval in_gate_proj = in_gate + x.domain().diam() * v.codomain();
    out_gate &= in_gate_proj;
    ctc |= out_gate != x.outputGate();
    x.setOutputGate(out_gate);

    Interval out_gate_proj = out_gate - x.domain().diam() * v.codomain();
    in_gate &= out_gate_proj;
    ctc |= in_gate != x.inputGate();
    x.setInputGate(in_gate);

    return ctc;
  }

  Interval yilb(const Interval& t, const TubeSlice& x, const TubeSlice& v)
  {
    return x.inputGate().lb() + v.codomain().lb() * (t - x.domain().lb());
  }

  Interval yilb_inv(const Interval& y, const TubeSlice& x, const TubeSlice& v)
  {
    return ((y - x.inputGate().lb()) / v.codomain().lb()) + x.domain().lb();
  }

  Interval yiub(const Interval& t, const TubeSlice& x, const TubeSlice& v)
  {
    return x.inputGate().ub() + v.codomain().ub() * (t - x.domain().lb());
  }

  Interval yiub_inv(const Interval& y, const TubeSlice& x, const TubeSlice& v)
  {
    return ((y - x.inputGate().ub()) / v.codomain().ub()) + x.domain().lb();
  }

  Interval yolb(const Interval& t, const TubeSlice& x, const TubeSlice& v)
  {
    return x.outputGate().lb() + v.codomain().ub() * (t - x.domain().ub());
  }

  Interval yolb_inv(const Interval& y, const TubeSlice& x, const TubeSlice& v)
  {
    return ((y - x.outputGate().lb()) / v.codomain().ub()) + x.domain().ub();
  }

  Interval youb(const Interval& t, const TubeSlice& x, const TubeSlice& v)
  {
    return x.outputGate().ub() + v.codomain().lb() * (t - x.domain().ub());
  }

  Interval youb_inv(const Interval& y, const TubeSlice& x, const TubeSlice& v)
  {
    return ((y - x.outputGate().ub()) / v.codomain().lb()) + x.domain().ub();
  }

  typedef Interval (* vFunctionCall)(const Interval& t, const TubeSlice& x, const TubeSlice& v);

  Interval linesIntersectionUb(const TubeSlice& x, const TubeSlice& v)
  {
    return (x.outputGate().ub() - x.inputGate().ub()
            + v.codomain().ub()*x.domain().lb()
            - v.codomain().lb()*x.domain().ub()) / v.codomain().diam();
  }

  Interval linesIntersectionLb(const TubeSlice& x, const TubeSlice& v)
  {
    return (x.inputGate().lb() - x.outputGate().lb()
            + v.codomain().ub()*x.domain().ub()
            - v.codomain().lb()*x.domain().lb()) / v.codomain().diam();
  }

  ConvexPolygon CtcDeriv::getPolygon(const TubeSlice& x, const TubeSlice& v)
  {
    if(v.codomain().is_unbounded())
    {
      //throw Exception("CtcDeriv::getPolygon", "unbounded derivative");
    }

    else if(x.inputGate() == Interval::ALL_REALS || x.outputGate() == Interval::ALL_REALS)
    {
      //throw Exception("CtcDeriv::getPolygon", "unbounded gates");
    }

    else
    {
      vector<Point> v_pts;

      v_pts.push_back(point(x.domain().lb(), x.inputGate().lb()));
      v_pts.push_back(point(x.domain().lb(), x.inputGate().ub()));

      Interval t_inter_ub = linesIntersectionUb(x, v);
      if(t_inter_ub.intersects(x.domain()))
        v_pts.push_back(point(t_inter_ub.mid() ,youb(t_inter_ub.mid(), x, v).mid()));

      v_pts.push_back(point(x.domain().ub(), x.outputGate().ub()));
      v_pts.push_back(point(x.domain().ub(), x.outputGate().lb()));

      Interval t_inter_lb = linesIntersectionLb(x, v);
      if(t_inter_lb.intersects(x.domain()))
        v_pts.push_back(point(t_inter_lb.mid(), yolb(t_inter_lb.mid(), x, v).mid()));

      ConvexPolygon p(v_pts);
      return p;
    }
  }

  bool CtcDeriv::contractEnvelope(const TubeSlice& x, const TubeSlice& v, Interval& t, Interval& y)
  {
    t &= x.domain();
    DomainException::check(x, t);
    Interval prev_y = y;

    if(t.is_degenerated() && (t.lb() == x.domain().lb() || t.lb() == x.domain().ub()))
      y &= x[t.lb()];

    else
    {
      Interval envelope;

      if(x.inputGate() == Interval::ALL_REALS || x.outputGate() == Interval::ALL_REALS)
        envelope = Interval::ALL_REALS;

      else if(v.codomain().is_degenerated())
        envelope = yiub(t, x, v) | yilb(t, x, v);

      else
      {
        if(v.codomain() == Interval::ALL_REALS)
          envelope = Interval::ALL_REALS;

        else if(v.codomain().lb() == NEG_INFINITY)
          envelope = yiub(t.ub(), x, v) | yolb(t.lb(), x, v);

        else if(v.codomain().ub() == POS_INFINITY)
          envelope = yilb(t.ub(), x, v) | youb(t.lb(), x, v);

        else
        {
          envelope.set_empty();

          Interval t_inter_ub = linesIntersectionUb(x, v);

          if(t_inter_ub.intersects(t))
            envelope |= youb(t_inter_ub, x, v);

          if(t_inter_ub.ub() <= t.lb())
            envelope |= youb(t, x, v);

          if(t_inter_ub.lb() >= t.ub())
            envelope |= yiub(t, x, v);

          Interval t_inter_lb = linesIntersectionLb(x, v);

          if(t_inter_lb.intersects(t))
            envelope |= yolb(t_inter_lb, x, v);

          if(t_inter_lb.ub() <= t.lb())
            envelope |= yolb(t, x, v);

          if(t_inter_lb.lb() >= t.ub())
            envelope |= yilb(t, x, v);
        }
      }

      envelope &= x.codomain();
      y &= envelope;
    }

    return prev_y != y;
  }
}