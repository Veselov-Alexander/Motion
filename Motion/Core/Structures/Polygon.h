#pragma once

#include "CGAL/Exact_predicates_exact_constructions_kernel.h"
#include "CGAL/Polygon_2.h"
#include "CGAL/Polygon_with_holes_2.h"
#include "CGAL/Vector_2.h"

#include "QPainterPath"
#include "QPolygonF"
#include "QPointF"
#include "QLineF"
#include "QRect"

#include <vector>


namespace
{
    typedef CGAL::Exact_predicates_exact_constructions_kernel   Kernel;
    typedef Kernel::Point_2                                     Point_2;
    typedef Kernel::Segment_2                                   Segment_2;
    typedef CGAL::Polygon_2<Kernel>                             Polygon_2;
    typedef CGAL::Polygon_with_holes_2<Kernel>                  Polygon_with_holes_2;
    typedef Polygon_2::Vertex_iterator                          Iterator_2;
}

class Polygon
{
public:
    explicit Polygon(const QPolygonF& polygon);
    bool inside(const QPointF& point) const;
    bool intersects(const QLineF& line, bool bStrict = false) const;
    bool isSimple() const;
    bool unite(const Polygon& polygon);
    Polygon united(const Polygon& polygon) const;
    Polygon minkowskiSum(const Polygon& polygon) const;
    QPainterPath toPath() const;
    std::vector<QPointF> points() const;
    std::vector<std::vector<QPointF>> holes() const;
    QRectF bounds() const;
private:
    explicit Polygon(const Polygon_2& polygon);
    explicit Polygon(const Polygon_with_holes_2& polygon);
    QRectF boundsRect(const QPolygonF& polygon);
    bool intersectsPolygon_2(const Segment_2& segment, const Polygon_2& polygon, bool bStrict = false) const;
private:
    Polygon_2 m_polygon;
    Polygon_with_holes_2 m_polygonWithHoles;
    QRectF m_boundsRect;
    bool m_bWithHoles;
};


class PolygonSet
{
public:
    void insert(const Polygon& polygon);
    bool inside(const QPointF& point) const;
    bool intersects(const QLineF& line, bool bStrict = false) const;
    void clear();
    size_t size() const;
    Polygon& operator[](size_t index);
    Polygon operator[](size_t index) const;
    std::vector<QPointF> points() const;
    std::vector<QLineF> lines() const;
    std::vector<Polygon>::iterator begin();
    std::vector<Polygon>::iterator end();
private:
    std::vector<Polygon> m_polygons;
};