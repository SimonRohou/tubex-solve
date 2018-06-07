/* ============================================================================
 *  tubex-lib - Polygon class
 * ============================================================================
 *  Copyright : Copyright 2017 Simon Rohou
 *  License   : This program is distributed under the terms of
 *              the GNU Lesser General Public License (LGPL).
 *
 *  Author(s) : Simon Rohou
 *  Bug fixes : -
 *  Created   : 2018
 * ---------------------------------------------------------------------------- */

#include <iostream>
#include "tubex_Polygon.h"
#include "tubex_Exception.h"

using namespace std;
using namespace ibex;

#ifdef _TUBES_POLYG_WITH_BOOST_

  #include <deque>
  #include <boost/geometry.hpp>
  #include <boost/foreach.hpp>

  using boost::geometry::get;
  typedef boost::geometry::model::d2::point_xy<double> point;
  typedef boost::geometry::model::polygon<point> polygon;
  typedef boost::geometry::model::linestring<point> linestring;

  namespace tubex
  {
    Polygon::Polygon()
    {

    }

    Polygon::Polygon(const polygon& p)
    {
      m_polygon = p;
      makeConvex();
    }

    Polygon::Polygon(const IntervalVector& box)
    {
      vector<IntervalVector> v_boxes;
      v_boxes.push_back(box);
      createFromBoxes(v_boxes);
    }

    Polygon::Polygon(const vector<IntervalVector>& v_boxes)
    {
      createFromBoxes(v_boxes);
    }

    Polygon::Polygon(const vector<double>& v_x, const vector<double>& v_y)
    {
      createFromPoints(v_x, v_y);
    }

    int Polygon::nbPoints() const
    {
      return m_polygon.outer().size();
    }

    void Polygon::getPoints(vector<double>& v_x, vector<double>& v_y) const
    {
      v_x.clear();
      v_y.clear();

      for(int i = 0 ; i < m_polygon.outer().size() ; i++)
      {
        v_x.push_back(get<0>(m_polygon.outer()[i]));
        v_y.push_back(get<1>(m_polygon.outer()[i]));
      }
    }

    IntervalVector Polygon::box() const
    {
      IntervalVector iv_box(2);
      boost::geometry::model::box<point> box;
      boost::geometry::envelope(m_polygon, box);
      iv_box[0] = Interval(get<0>(box.min_corner()), get<0>(box.max_corner()));
      iv_box[1] = Interval(get<1>(box.min_corner()), get<1>(box.max_corner()));
      return iv_box;
    }

    void Polygon::makeConvex()
    {
      boost::geometry::model::polygon<boost::geometry::model::d2::point_xy<double> > poly;
      boost::geometry::convex_hull(m_polygon, poly);
      m_polygon = poly;
    }
    
    bool Polygon::operator==(const Polygon& p) const
    {
      return boost::geometry::equals(m_polygon, p.m_polygon);
    }
    
    bool Polygon::operator!=(const Polygon& p) const
    {
      return !operator==(p);
    }

    ostream& operator<<(ostream& str, const Polygon& p)
    {
      vector<double> v_x, v_y;
      p.getPoints(v_x, v_y);

      str << "{";
      for(int i = 0 ; i < v_x.size() ; i++)
      {
        if(i != 0) str << ",";
        str << "(" << v_x[i] << "," << v_y[i] << ")";
      }

      str << "}";
      return str;
    }

    Polygon Polygon::translate(const Polygon& p, const IntervalVector& translation)
    {
      if(translation.is_unbounded())
        throw Exception("Polygon::translate", "unbounded translation");

      vector<point> const& points = p.m_polygon.outer();
      vector<IntervalVector> v_boxes;

      for(vector<point>::size_type i = 0; i < points.size(); ++i)
      {
        IntervalVector point_box(2);
        point_box[0] = Interval(get<0>(points[i]));
        point_box[1] = Interval(get<1>(points[i]));
        point_box += translation;
        v_boxes.push_back(point_box);
      }

      return Polygon(v_boxes);
    }

    void Polygon::createFromPoints(const vector<double>& v_x, const vector<double>& v_y)
    {
      if(v_x.size() != v_y.size())
        throw Exception("Polygon::createFromPoints", "points vectors of different size");

      if(v_x.size() == 0)
        throw Exception("Polygon::createFromPoints", "empty points vectors");

      vector<point> pts;
      for(int i = 0 ; i < v_x.size() ; i++)
        pts.push_back(point(v_x[i], v_y[i]));
      boost::geometry::clear(m_polygon);
      boost::geometry::assign_points(m_polygon, pts);
      makeConvex();
    }

    void Polygon::createFromBoxes(const vector<IntervalVector>& v_boxes)
    {
      if(v_boxes.size() == 0)
        throw Exception("Polygon::createFromBoxes", "empty boxes vector");

      vector<point> pts;
      for(int i = 0 ; i < v_boxes.size() ; i++)
      {
        if(v_boxes[i].is_unbounded())
          throw Exception("Polygon::createFromBoxes", "unbounded box");

        pts.push_back(point(v_boxes[i][0].lb(), v_boxes[i][1].lb()));
        pts.push_back(point(v_boxes[i][0].ub(), v_boxes[i][1].lb()));
        pts.push_back(point(v_boxes[i][0].lb(), v_boxes[i][1].ub()));
        pts.push_back(point(v_boxes[i][0].ub(), v_boxes[i][1].ub()));
      }

      boost::geometry::model::polygon<boost::geometry::model::d2::point_xy<double> > poly;
      boost::geometry::clear(m_polygon);
      boost::geometry::assign_points(m_polygon, pts);
      makeConvex();
    }
    
    Polygon operator&(const Polygon& p1, const Polygon& p2)
    {
      /*if(p1.nbPoints() == 2)
      {
        linestring ls;
        boost::geometry::append(ls, point(get<0>(p1.m_polygon.outer()[0]), get<1>(p1.m_polygon.outer()[0])));
        boost::geometry::append(ls, point(get<0>(p1.m_polygon.outer()[1]), get<1>(p1.m_polygon.outer()[1])));
        
        deque<linestring> output;
        boost::geometry::intersection(p2.m_polygon, ls, output);
        BOOST_FOREACH(linestring const& l, output)
        {
          return Polygon(polygon(l));
        }
      }*/

      deque<polygon> output;
      boost::geometry::intersection(p1.m_polygon, p2.m_polygon, output);
      BOOST_FOREACH(polygon const& p, output)
        return Polygon(p);
    }
  }

#endif