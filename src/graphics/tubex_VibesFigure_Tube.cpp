/* ============================================================================
 *  tubex-lib - VibesFigure_Tube class
 * ============================================================================
 *  Copyright : Copyright 2017 Simon Rohou
 *  License   : This program is distributed under the terms of
 *              the GNU Lesser General Public License (LGPL).
 *
 *  Author(s) : Simon Rohou
 *  Bug fixes : -
 *  Created   : 2015
 * ---------------------------------------------------------------------------- */

#include "ibex_Interval.h"
#include "ibex_IntervalVector.h"
#include "vibes.h"
#include "tubex_Polygon.h"
#include "tubex_VibesFigure_Tube.h"
#include "tubex_VibesFigure_Polygon.h"
#include "tubex_DimensionException.h"

using namespace std;
using namespace ibex;

namespace tubex
{
  // Static items

    vector<VibesFigure_Tube*> VibesFigure_Tube::v_vibesfig_tube;

    void VibesFigure_Tube::draw(const string& fig_name, int x, int y)
    {
      draw(fig_name, NULL, NULL, x, y);
    }

    void VibesFigure_Tube::draw(const string& fig_name, const TubeVector *tube, int x, int y)
    {
      draw(fig_name, tube, NULL, x, y);
    }

    void VibesFigure_Tube::draw(const string& fig_name, const TrajectoryVector *traj, int x, int y)
    {
      draw(fig_name, NULL, traj, x, y);
    }
    
    void VibesFigure_Tube::draw(const string& fig_name, const TubeVector *tube, const TrajectoryVector *traj, int x, int y)
    {
      if(VibesFigure_Tube::v_vibesfig_tube.size() == 0)
      {
        vibes::beginDrawing();
        vibes::axisAuto();
      }

      VibesFigure_Tube *figtube = NULL;
      for(int i = 0 ; i < v_vibesfig_tube.size() ; i++)
      {
        if(v_vibesfig_tube[i]->name() == fig_name)
        {
          figtube = v_vibesfig_tube[i];
          break;
        }
      }

      if(figtube == NULL)
      {
        figtube = new VibesFigure_Tube(fig_name);
        v_vibesfig_tube.push_back(figtube);
      }

      if(tube != NULL) DimensionException::check(*figtube, *tube);
      if(tube != NULL && traj != NULL) DimensionException::check(*tube, *traj);

      figtube->set_properties(x, y, 700, 350);
      if(tube != NULL) figtube->add_tube(tube, DEFAULT_TUBE_NAME);
      if(traj != NULL) figtube->add_trajectory(traj, DEFAULT_TRAJ_NAME);
      figtube->show();
    }

    void VibesFigure_Tube::end_drawing()
    {
      for(int i = 0 ; i < v_vibesfig_tube.size() ; i++)
        delete v_vibesfig_tube[i];
      vibes::endDrawing();
    }

  // Non-static items

    VibesFigure_Tube::VibesFigure_Tube(const string& fig_name, int dim)
      : VibesFigure(fig_name, dim)
    {
      DimensionException::check(dim);
      m_view_box = IntervalVector(2, Interval::EMPTY_SET);
    }

    VibesFigure_Tube::VibesFigure_Tube(const string& fig_name, const TubeVector *tube, const TrajectoryVector *traj)
      : VibesFigure_Tube(fig_name, tube->dim())
    {
      DimensionException::check(*this, *tube);
      DimensionException::check(*this, *traj);
      add_tube(tube, DEFAULT_TUBE_NAME);
      if(traj != NULL) add_trajectory(traj, DEFAULT_TUBE_NAME);
    }

    VibesFigure_Tube::~VibesFigure_Tube()
    {
      typename map<const TubeVector*,FigTubeParams>::const_iterator it;
      for(it = m_map_tubes.begin(); it != m_map_tubes.end(); it++)
        if(it->second.tube_copy != NULL)
          delete it->second.tube_copy;
    }

    int VibesFigure_Tube::dim() const
    {
      if(m_map_tubes.empty())
      {
        if(m_map_trajs.empty())
          return 0;

        return m_map_trajs.begin()->first->dim();
      }

      else
        return m_map_tubes.begin()->first->dim();
    }

    void VibesFigure_Tube::add_tube(const TubeVector *tube, const string& name,
                                   const string& color_frgrnd, const string& color_bckgrnd)
    {
      DimensionException::check(*this, *tube);

      if(m_map_tubes.find(tube) != m_map_tubes.end())
        cout << "Warning VibesFigure_Tube::add_tube(): tube already added" << endl;

      else
        m_map_tubes[tube];

      set_tube_name(tube, name);
      set_tube_color(tube, color_frgrnd, color_bckgrnd);
      set_tube_color(tube, TubeColorType::SLICES, DEFAULT_SLICES_COLOR);
      set_tube_color(tube, TubeColorType::GATES, DEFAULT_GATES_COLOR);
      set_tube_color(tube, TubeColorType::POLYGONS, DEFAULT_POLYGONS_COLOR);
    }

    void VibesFigure_Tube::set_tube_name(const TubeVector *tube, const string& name)
    {
      DimensionException::check(*this, *tube);

      if(m_map_tubes.find(tube) == m_map_tubes.end())
        cout << "Warning VibesFigure_Tube::set_tube_name(): unknown tube" << endl;

      m_map_tubes[tube].name = name;
    }
    
    void VibesFigure_Tube::set_tube_derivative(const TubeVector *tube, const TubeVector *derivative)
    {
      DimensionException::check(*this, *derivative);

      if(m_map_tubes.find(tube) == m_map_tubes.end())
        cout << "Warning VibesFigure_Tube::set_tube_derivative(): unknown tube" << endl;

      m_map_tubes[tube].tube_derivative = derivative;
    }

    void VibesFigure_Tube::set_tube_color(const TubeVector *tube, const string& color_frgrnd, const string& color_bckgrnd)
    {
      if(m_map_tubes.find(tube) == m_map_tubes.end())
        cout << "Warning VibesFigure_Tube::set_tube_color(): unknown tube" << endl;

      set_tube_color(tube, TubeColorType::FOREGROUND, color_frgrnd);
      set_tube_color(tube, TubeColorType::BACKGROUND, color_bckgrnd);
    }

    void VibesFigure_Tube::set_tube_color(const TubeVector *tube, int color_type, const string& color)
    {
      if(m_map_tubes.find(tube) == m_map_tubes.end())
        cout << "Warning VibesFigure_Tube::set_tube_color(): unknown tube" << endl;

      m_map_tubes[tube].m_colors[color_type] = color;
    }
    
    void VibesFigure_Tube::remove_tube(const TubeVector *tube)
    {
      if(m_map_tubes.find(tube) == m_map_tubes.end())
        cout << "Warning VibesFigure_Tube::remove_tube(): unable to remove" << endl;

      if(m_map_tubes[tube].tube_copy != NULL)
        delete m_map_tubes[tube].tube_copy;
      m_map_tubes.erase(tube);
    }

    void VibesFigure_Tube::add_trajectory(const TrajectoryVector *traj, const string& name, const string& color)
    {
      DimensionException::check(*this, *traj);

      if(m_map_trajs.find(traj) != m_map_trajs.end())
        cout << "Warning VibesFigure_Tube::add_trajectory(): trajectory already added" << endl;

      else
        m_map_trajs[traj].color = color;

      set_trajectory_name(traj, name);
    }

    void VibesFigure_Tube::set_trajectory_name(const TrajectoryVector *traj, const string& name)
    {
      if(m_map_trajs.find(traj) == m_map_trajs.end())
        cout << "Warning VibesFigure_Tube::set_trajectory_name(): unknown trajectory" << endl;

      m_map_trajs[traj].name = name;
    }
    
    void VibesFigure_Tube::set_trajectory_color(const TrajectoryVector *traj, const string& color)
    {
      if(m_map_trajs.find(traj) == m_map_trajs.end())
        cout << "Warning VibesFigure_Tube::set_trajectory_color(): unknown trajectory" << endl;

      m_map_trajs[traj].color = color;
    }
    
    void VibesFigure_Tube::remove_trajectory(const TrajectoryVector *traj)
    {
      if(m_map_trajs.find(traj) == m_map_trajs.end())
        cout << "Warning VibesFigure_Tube::remove_trajectory(): unable to remove" << endl;

      m_map_trajs.erase(traj);
    }
    
    void VibesFigure_Tube::show()
    {
      for(int i = 0 ; i < m_nb_layers ; i++)
        show(i, false);
    }
    
    void VibesFigure_Tube::show(bool detail_slices)
    {
      for(int i = 0 ; i < m_nb_layers ; i++)
        show(i, detail_slices);
    }

    void VibesFigure_Tube::show(int dim, bool detail_slices)
    {
      set_current_layer(dim);

      typename map<const TubeVector*,FigTubeParams>::const_iterator it_tubes;
      for(it_tubes = m_map_tubes.begin(); it_tubes != m_map_tubes.end(); it_tubes++)
        m_view_box |= draw_tube(it_tubes->first, detail_slices);

      // Trajectories are drawn on top of the tubes
      typename map<const TrajectoryVector*,FigTrajParams>::const_iterator it_trajs;
      for(it_trajs = m_map_trajs.begin(); it_trajs != m_map_trajs.end(); it_trajs++)
        m_view_box |= draw_trajectory(it_trajs->first);

      axis_limits(m_view_box);

      // The following is used as a calibration of the SVG file
      vibes::clearGroup(name(), "transparent_box");
      vibes::drawBox(m_view_box, vibesParams("figure", name(), "group", "transparent_box"));
    }

    const IntervalVector VibesFigure_Tube::draw_tube(const TubeVector *tube, bool detail_slices)
    {
      IntervalVector viewbox(2, Interval::EMPTY_SET);

      // Computing viewbox

        double image_lb, image_ub;

        if(!tube->codomain().is_unbounded())
        {
          image_lb = tube->codomain()[m_current_layer].lb();
          image_ub = tube->codomain()[m_current_layer].ub();
        }

        else // some slices can be [-oo,+oo], maybe not all of them
        {
          image_lb = NAN;
          image_ub = NAN;

          for(int i = 0 ; i < tube->nb_slices() ; i++)
          {
            Interval slice = (*tube)[i][m_current_layer];
            if(!slice.is_unbounded())
            {
              image_lb = std::isnan(image_lb) || image_lb > slice.lb() ? slice.lb() : image_lb;
              image_ub = std::isnan(image_ub) || image_ub < slice.ub() ? slice.ub() : image_ub;
            }
          }
        }

        viewbox[0] = tube->domain();
        viewbox[1] = Interval(image_lb, image_ub);

      // Displaying tube

        ostringstream o;
        o << "tube_" << m_map_tubes[tube].name;
        string group_name = o.str();
        string group_name_bckgrnd = o.str() + "_old";

        // Two display modes available:
        // - one in which each slice is shown
        // - one in which only the polygon envelope of the tube is shown

        // First, displaying background.
        // The background is the previous version of the tube (before contraction).
        // Always displayed as a polygon.
        {
          if(m_map_tubes[tube].tube_copy == NULL)
          {
            // If a copy of the tube has not been done,
            // we make one and no display is done.
          }

          else
          {
            // Otherwise, the copy is displayed and then updated
            // with the current version of the tube.

            vibes::clearGroup(name(), group_name_bckgrnd);
            vibes::newGroup(group_name_bckgrnd, m_map_tubes[tube].m_colors[TubeColorType::BACKGROUND], vibesParams("figure", name()));
            vibes::Params params_background = vibesParams("figure", name(), "group", group_name_bckgrnd);
            VibesFigure_Polygon::draw(polygon_envelope(m_map_tubes[tube].tube_copy), params_background);

            delete m_map_tubes[tube].tube_copy;
          }
          
          m_map_tubes[tube].tube_copy = new TubeVector(*tube);
        }

        // Second, the foreground: actual values of the tube.
        // Can be either displayed slice by slice or with a polygon envelope.

        vibes::Params params_foreground = vibesParams("figure", name(), "group", group_name);
        vibes::Params params_foreground_polygons = vibesParams("figure", name(), "group", group_name + "_polygons");
        vibes::Params params_foreground_gates = vibesParams("figure", name(), "group", group_name + "_gates", "FixedScale", true);
        vibes::clearGroup(name(), group_name);
        vibes::clearGroup(name(), group_name + "_polygons");
        vibes::clearGroup(name(), group_name + "_gates");

        if(detail_slices)
        {
          vibes::newGroup(group_name, m_map_tubes[tube].m_colors[TubeColorType::SLICES], vibesParams("figure", name()));
          vibes::newGroup(group_name + "_polygons", m_map_tubes[tube].m_colors[TubeColorType::POLYGONS], vibesParams("figure", name()));
          vibes::newGroup(group_name + "_gates", m_map_tubes[tube].m_colors[TubeColorType::GATES], vibesParams("figure", name()));
          
          const TubeSlice *slice = tube->get_first_slice();
          const TubeSlice *deriv_slice = NULL;

          if(m_map_tubes[tube].tube_derivative != NULL)
            deriv_slice = m_map_tubes[tube].tube_derivative->get_first_slice();

          draw_gate(slice->input_gate()[m_current_layer], tube->domain().lb(), params_foreground_gates);

          while(slice != NULL)
          {
            if(deriv_slice != NULL)
              draw_slice(*slice, *deriv_slice, params_foreground, params_foreground_polygons);
            else
              draw_slice(*slice, params_foreground);

            draw_gate(slice->output_gate()[m_current_layer], slice->domain().ub(), params_foreground_gates);
            slice = slice->next_slice();
            
            if(deriv_slice != NULL)
              deriv_slice = deriv_slice->next_slice();
          }
        }

        else
        {
          vibes::newGroup(group_name, m_map_tubes[tube].m_colors[TubeColorType::FOREGROUND], vibesParams("figure", name()));
          VibesFigure_Polygon::draw(polygon_envelope(tube), params_foreground);
        }

      return viewbox;
    }

    const Polygon VibesFigure_Tube::polygon_envelope(const TubeVector *tube)
    {
      if(tube->is_empty())
        cout << "Tube graphics: warning, tube " << m_map_tubes[tube].name << " is empty" << endl;

      vector<Point> v_pts;

      for(int i = 0 ; i < tube->nb_slices() ; i++)
      {
        IntervalVector slice_box = tube->get_slice(i)->box();
        slice_box[1 + m_current_layer] = trunc_inf(slice_box[1 + m_current_layer]);
        v_pts.push_back(Point(slice_box[0].lb(), slice_box[1 + m_current_layer].ub()));
        v_pts.push_back(Point(slice_box[0].ub(), slice_box[1 + m_current_layer].ub()));
      }

      for(int i = tube->nb_slices() - 1 ; i >= 0 ; i--)
      {
        IntervalVector slice_box = tube->get_slice(i)->box();
        slice_box[1 + m_current_layer] = trunc_inf(slice_box[1 + m_current_layer]);
        v_pts.push_back(Point(slice_box[0].ub(), slice_box[1 + m_current_layer].lb()));
        v_pts.push_back(Point(slice_box[0].lb(), slice_box[1 + m_current_layer].lb()));
      }
      
      return Polygon(v_pts);
    }

    void VibesFigure_Tube::draw_slice(const TubeSlice& slice, const vibes::Params& params) const
    {
      if(slice.codomain().is_empty())
        return; // no display

      IntervalVector boundedSlice(2);
      boundedSlice[0] = slice.domain();
      boundedSlice[1] = trunc_inf(slice.codomain()[m_current_layer]);
      vibes::drawBox(boundedSlice, params);
    }

    void VibesFigure_Tube::draw_slice(const TubeSlice& slice, const TubeSlice& deriv_slice, const vibes::Params& params_slice, const vibes::Params& params_polygon) const
    {
      if(slice.codomain().is_empty())
        return; // no display

      draw_slice(slice, params_slice);
      VibesFigure_Polygon::draw(slice.polygon(0, deriv_slice), params_polygon);
    }

    void VibesFigure_Tube::draw_gate(const Interval& gate, double t, const vibes::Params& params) const
    {
      if(gate.is_empty())
        return; // no display
      
      if(gate.is_degenerated())
      {
        vibes::drawPoint(t, gate.lb(), 1, params);
      }

      else
      {
        IntervalVector gateBox(2);
        gateBox[0] = t;
        gateBox[1] = trunc_inf(gate);
        vibes::drawBox(gateBox, params);
      }
    }
    
    const IntervalVector VibesFigure_Tube::draw_trajectory(const TrajectoryVector *traj, float points_size)
    {
      IntervalVector viewbox(2, Interval::EMPTY_SET);

      std::ostringstream o;
      o << "traj_" << m_map_trajs[traj].name;
      vibes::newGroup(o.str(), m_map_trajs[traj].color, vibesParams("figure", name()));

      if(traj->domain().is_unbounded() || traj->domain().is_empty())
        return viewbox;

      // Two display modes available:
      // - one by Fnc
      // - one by map<double,double>

      vector<double> v_x, v_y;

      if(traj->get_function() == NULL)
      {
        typename map<double,Vector>::const_iterator it_scalar_values;
        for(it_scalar_values = traj->get_map().begin(); it_scalar_values != traj->get_map().end(); it_scalar_values++)
        {
          if(points_size != 0.)
            vibes::drawPoint(it_scalar_values->first, it_scalar_values->second[m_current_layer],
                             points_size, vibesParams("figure", name(), "group", o.str()));

          else
          {
            v_x.push_back(it_scalar_values->first);
            v_y.push_back(it_scalar_values->second[m_current_layer]);
          }

          viewbox[0] |= it_scalar_values->first;
          viewbox[1] |= it_scalar_values->second[m_current_layer];
        }
      }

      else
      {
        for(double t = traj->domain().lb() ; t <= traj->domain().ub() ; t+=traj->domain().diam()/TRAJ_NB_DISPLAYED_POINTS)
        {
          if(points_size != 0.)
            vibes::drawPoint(t, (*traj)[t][m_current_layer], points_size, vibesParams("figure", name(), "group", o.str()));

          else
          {
            v_x.push_back(t);
            v_y.push_back((*traj)[t][m_current_layer]);
          }

          viewbox[0] |= t;
          viewbox[1] |= (*traj)[t][m_current_layer];
        }
      }

      if(v_x.size() != 0)
        vibes::drawLine(v_x, v_y, vibesParams("figure", name(), "group", o.str()));

      return viewbox;
    }
}