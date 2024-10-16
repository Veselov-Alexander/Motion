#include "Polygon.h"

#include "Core/Algorithms/Utils.h"

#include "CGAL/Boolean_set_operations_2.h"
#include "CGAL/minkowski_sum_2.h"

#include <boost/geometry/geometry.hpp>
#include <boost/polygon/polygon.hpp>

#include <functional>
#include <execution>

typedef boost::geometry::model::d2::point_xy<qreal> point_xy;
typedef boost::geometry::model::linestring<point_xy> Linestring;


inline Point_2 toPoint_2(const QPointF& point)
{
    return Point_2(point.x(), point.y());
}

inline QPointF toQPointF(const Point_2& point)
{
    return QPointF(CGAL::to_double(point.x()),
                   CGAL::to_double(point.y()));
}

inline QPolygonF toQPolygonF(const Polygon_2::Vertex_iterator begin, const Polygon_2::Vertex_iterator end)
{
    QPolygonF result;
    for (auto it = begin; it != end; ++it)
    {
        result.append(toQPointF(*it));
    }
    return result;
}

inline QRectF toQRectF(const CGAL::Bbox_2& bbox)
{
    return QRectF(QPointF(bbox.xmin(), bbox.ymin()),
                  QPointF(bbox.xmax(), bbox.ymax()));
}

inline bool insideRect(const QPointF& point, const QRectF& rect)
{
    return point.x() >= rect.x() &&
           point.x() <= rect.x() + rect.width() &&
           point.y() >= rect.y() &&
           point.y() <= rect.y() + rect.height();
}

Polygon::Polygon() {}

Polygon::Polygon(const QPolygonF& polygon, bool withHoles)
    : m_bWithHoles(withHoles), m_boundsRect(boundsRect(polygon))
{
    try {
        auto& points = withHoles ? m_polygonWithHoles.outer_boundary() : m_polygon;

        for (int i = 0; i < polygon.size(); ++i)
        {
            if (i - 1 > 0 && polygon[i - 1] == polygon[i])
                continue;
            points.push_back(toPoint_2(polygon[i]));
        }

        if (points.orientation() != CGAL::Sign::COUNTERCLOCKWISE)
        {
            points.reverse_orientation();
        }
        
    }
    catch (...) {}
}

Polygon::Polygon(const Polygon_2& polygon)
    : m_polygon(polygon), m_bWithHoles(false), m_boundsRect(toQRectF(polygon.bbox()))
{
}

Polygon::Polygon(const Polygon_with_holes_2& polygon)
    : m_polygonWithHoles(polygon), m_bWithHoles(true), m_boundsRect(toQRectF(polygon.bbox()))
{
}

QRectF Polygon::boundsRect(const QPolygonF& polygon)
{
    if (polygon.isEmpty())
        return {};

    qreal xmin, xmax, ymin, ymax;

    xmin = xmax = polygon[0].x();
    ymin = ymax = polygon[0].y();

    for (int i = 1; i < polygon.size(); ++i)
    {
        const qreal x = polygon[i].x();
        const qreal y = polygon[i].y();

        xmin = std::min(xmin, x);
        xmax = std::max(xmax, x);
        ymin = std::min(ymin, y);
        ymax = std::max(ymax, y);
    }

    return QRectF(QPointF(xmin, ymin), QPointF(xmax, ymax));
}

bool Polygon::unite(const Polygon& polygon)
{
    bool bRet = false;

    Polygon_with_holes_2 result;

    if (!m_bWithHoles && !polygon.m_bWithHoles)
    {
        bRet = CGAL::join(m_polygon, polygon.m_polygon, result);
    }
    else if (m_bWithHoles && polygon.m_bWithHoles)
    {
        bRet = CGAL::join(m_polygonWithHoles, polygon.m_polygonWithHoles, result);
    }
    else if (!m_bWithHoles && polygon.m_bWithHoles)
    {
        bRet = CGAL::join(m_polygon, polygon.m_polygonWithHoles, result);
    }
    else // if (bWithHoles && !polygon.bWithHoles)
    {
        bRet = CGAL::join(m_polygonWithHoles, polygon.m_polygon, result);
    }

    if (bRet)
        m_polygonWithHoles = result;

    m_bWithHoles = true;
    m_boundsRect = boundsRect(toQRectF(m_polygonWithHoles.bbox()));

    return bRet;
}

Polygon Polygon::united(const Polygon& polygon) const
{
    Polygon_with_holes_2 result;

    if (!m_bWithHoles && !polygon.m_bWithHoles)
    {
        CGAL::join(m_polygon, polygon.m_polygon, result);
    }
    else if (m_bWithHoles && polygon.m_bWithHoles)
    {
        CGAL::join(m_polygonWithHoles, polygon.m_polygonWithHoles, result);
    }
    else if(!m_bWithHoles && polygon.m_bWithHoles)
    {
        CGAL::join(m_polygon, polygon.m_polygonWithHoles, result);
    }
    else if (m_bWithHoles && !polygon.m_bWithHoles)
    {
        CGAL::join(m_polygonWithHoles, polygon.m_polygon, result);
    }

    return Polygon(result);
}

PolygonSet Polygon::subtracted(const Polygon& polygon) const
{
    std::list<Polygon_with_holes_2> result;

    if (!m_bWithHoles && !polygon.m_bWithHoles)
    {
        CGAL::difference(m_polygon, polygon.m_polygon, std::back_inserter(result));
    }
    else if (m_bWithHoles && polygon.m_bWithHoles)
    {
        CGAL::difference(m_polygonWithHoles, polygon.m_polygonWithHoles, std::back_inserter(result));
    }
    else if (!m_bWithHoles && polygon.m_bWithHoles)
    {
        CGAL::difference(m_polygon, polygon.m_polygonWithHoles, std::back_inserter(result));
    }
    else if (m_bWithHoles && !polygon.m_bWithHoles)
    {
        CGAL::difference(m_polygonWithHoles, polygon.m_polygon, std::back_inserter(result));
    }

    PolygonSet s;
    for (auto r : result)
    {
        s.insert(Polygon(r));
    }

    return s;
}

PolygonSet Polygon::intersected(const Polygon& polygon) const
{
    std::list<Polygon_with_holes_2> result;

    if (!m_bWithHoles && !polygon.m_bWithHoles)
    {
        CGAL::intersection(m_polygon, polygon.m_polygon, std::back_inserter(result));
    }
    else if (m_bWithHoles && polygon.m_bWithHoles)
    {
        CGAL::intersection(m_polygonWithHoles, polygon.m_polygonWithHoles, std::back_inserter(result));
    }
    else if (!m_bWithHoles && polygon.m_bWithHoles)
    {
        CGAL::intersection(m_polygon, polygon.m_polygonWithHoles, std::back_inserter(result));
    }
    else if (m_bWithHoles && !polygon.m_bWithHoles)
    {
        CGAL::intersection(m_polygonWithHoles, polygon.m_polygon, std::back_inserter(result));
    }

    PolygonSet s;
    for (auto r : result)
    {
        s.insert(Polygon(r));
    }

    return s;
}

Polygon Polygon::minkowskiSum(const Polygon& polygon) const
{
    if (!m_bWithHoles && !polygon.m_bWithHoles)
    {
        const Polygon_2& P = m_polygon;
        const Polygon_2& Q = polygon.m_polygon;

        assert(P.orientation() == CGAL::Sign::COUNTERCLOCKWISE);
        assert(Q.orientation() == CGAL::Sign::COUNTERCLOCKWISE);

        return Polygon(CGAL::minkowski_sum_2(P, Q));
    }
    else
    {
        assert(!"Not implemented");
        return Polygon(QPolygonF());
    }
}

QPolygonF Polygon::toPolygon(bool ignoreHoles) const
{
    auto outer = m_polygonWithHoles.outer_boundary();
    QPolygonF result = toQPolygonF(outer.vertices_begin(), outer.vertices_end());

    if (!ignoreHoles)
    {
        for (auto it = m_polygonWithHoles.holes_begin(); it != m_polygonWithHoles.holes_end(); ++it)
        {
            result = result.subtracted(toQPolygonF(it->vertices_begin(), it->vertices_end()));
        }
    }

    return result;
}

QPainterPath Polygon::toPath(bool ignoreHoles) const
{
    QPainterPath path;
    auto outer = m_polygonWithHoles.outer_boundary();
    path.addPolygon(toQPolygonF(outer.vertices_begin(), outer.vertices_end()));
    path.closeSubpath();

    if (!ignoreHoles)
    {
        for (auto it = m_polygonWithHoles.holes_begin(); it != m_polygonWithHoles.holes_end(); ++it)
        {
            QPainterPath inner;
            inner.addPolygon(toQPolygonF(it->vertices_begin(), it->vertices_end()));
            inner.closeSubpath();

            path = path.subtracted(inner);
        }
    }

    return path;
}

std::vector<QPointF> Polygon::points() const
{
    std::vector<QPointF> result;

    if (m_bWithHoles)
    {
        auto outer = m_polygonWithHoles.outer_boundary();

        for (auto& point : outer)
        {
            result.push_back(toQPointF(point));
        }
    }
    else
    {
        for (auto& point : m_polygon)
        {
            result.push_back(toQPointF(point));
        }
    }

    return result;
}

std::vector<std::vector<QPointF>> Polygon::holes() const
{
    std::vector<std::vector<QPointF>> holes;

    if (m_bWithHoles)
    {
        for (auto it = m_polygonWithHoles.holes_begin(); it != m_polygonWithHoles.holes_end(); ++it)
        {
            holes.push_back(std::vector<QPointF>());
            for (auto& point : *it)
            {
                holes.back().push_back(toQPointF(point));
            }
        }
    }

    return holes;
}

QRectF Polygon::bounds() const
{
    return m_boundsRect;
}

bool Polygon::inside(const QPointF& point, bool bStrict) const
{
    if (!insideRect(point, m_boundsRect))
    {
        return false;
    }

    const Point_2 mpoint = Point_2(point.x(), point.y());

    if (m_bWithHoles)
    {
        auto outer = m_polygonWithHoles.outer_boundary();

        switch (CGAL::bounded_side_2(outer.vertices_begin(), outer.vertices_end(), mpoint, Kernel()))
        {
        case CGAL::ON_UNBOUNDED_SIDE:
            return false;
        case CGAL::ON_BOUNDARY:
            if (bStrict)
                return false;
        case CGAL::ON_BOUNDED_SIDE:
            break;
            // point is inside outer
            // now need to check if point inside holes
        }

        for (auto holesIter = m_polygonWithHoles.holes_begin(); 
             holesIter != m_polygonWithHoles.holes_end(); ++holesIter)
        {
            auto hole = *holesIter;

            switch (CGAL::bounded_side_2(hole.vertices_begin(), hole.vertices_end(), mpoint, Kernel()))
            {
            case CGAL::ON_BOUNDED_SIDE:
                // point in inside hole
                // therefore, the point is not in polygon
                if (bStrict)
                    return false;
            case CGAL::ON_UNBOUNDED_SIDE:
            case CGAL::ON_BOUNDARY:
                break;
            }
        }
    }
    else
    {
        switch (CGAL::bounded_side_2(m_polygon.vertices_begin(), m_polygon.vertices_end(), mpoint, Kernel()))
        {
        case CGAL::ON_UNBOUNDED_SIDE:
            return false;
        case CGAL::ON_BOUNDARY:
            if (bStrict)
                return false;
        case CGAL::ON_BOUNDED_SIDE:
            break;
            // point is inside outer
            // now need to check if point inside holes
        }
    }

    return true;
}

bool Polygon::intersects(const QLineF& line, bool bStrict, std::vector<QPointF>* out) const
{
    assert(m_bWithHoles);

    boost::geometry::model::polygon<point_xy> polygon;

    auto outer = m_polygonWithHoles.outer_boundary();

    for (auto it = outer.begin(); it != outer.end(); ++it)
    {
        polygon.outer().push_back(point_xy(CGAL::to_double(it->x()), CGAL::to_double(it->y())));
    }

    polygon.outer().push_back(*polygon.outer().begin());

    for (auto IT = m_polygonWithHoles.holes_begin(); IT != m_polygonWithHoles.holes_end(); ++IT)
    {
        polygon.inners().push_back({});
        for (auto it = IT->vertices_begin(); it != IT->vertices_end(); ++it)
        {
            polygon.inners().back().push_back(point_xy(CGAL::to_double(it->x()), CGAL::to_double(it->y())));
        }
        polygon.inners().back().push_back(*polygon.inners().back().begin());
    }

    Linestring segment;

    segment.push_back(point_xy(line.p1().x(), line.p1().y()));
    segment.push_back(point_xy(line.p2().x(), line.p2().y()));

    bool intersects = false;

    if (bStrict)
    {
        intersects = !boost::geometry::touches(polygon, segment) && boost::geometry::intersects(polygon, segment);
    }
    else
    {
        intersects = boost::geometry::intersects(polygon, segment);
    }

    if (intersects && out)
    {
        std::vector<point_xy> result;
        boost::geometry::intersection(segment, polygon, result);
        std::vector<QPointF> r;
        for (const auto& p : result)
        {
            r.push_back(QPointF(p.x(), p.y()));
        }
        *out = r;
    }

    return intersects;
}

bool Polygon::isSimple() const
{
    try
    {
        if (m_bWithHoles)
        {
            return false;
        }
        else
        {
            return m_polygon.is_simple();
        }
    }
    catch (...)
    {
        return false;
    }
}

void PolygonSet::insert(const Polygon& polygon)
{
    std::vector<Polygon> result;
    Polygon unitedPolygon = polygon;

    std::vector<bool> objectsToDelete(m_polygons.size());

    for (int i = 0; i < m_polygons.size(); ++i)
    {
        objectsToDelete[i] = unitedPolygon.unite(m_polygons[i]);
    }

    for (int i = 0; i < m_polygons.size(); ++i)
    {
        if (!objectsToDelete[i])
        {
            result.push_back(m_polygons[i]);
        }
    }

    result.push_back(unitedPolygon);

    m_polygons = result;
}

bool PolygonSet::inside(const QPointF& point, bool bStrict, Polygon* out) const
{
    bool bFound = false;
    std::for_each(
        std::execution::par,
        m_polygons.begin(),
        m_polygons.end(),
        [&](auto&& polygon)
        {
            if (!bFound && polygon.inside(point, bStrict))
            {
                if (out)
                    *out = polygon;
                bFound = true;
                return;
            }
        });

    return bFound;
}

bool PolygonSet::intersects(const QLineF& line, bool bStrict) const
{
    bool bFound = false;
    std::for_each(
        std::execution::par,
        m_polygons.begin(),
        m_polygons.end(),
        [&](auto&& polygon)
        {
            if (!bFound && polygon.intersects(line, bStrict))
            {
                bFound = true;
                return;
            }
        });

    return bFound;
}

void PolygonSet::clear()
{
    m_polygons.clear();
}

size_t PolygonSet::size() const
{
    return m_polygons.size();
}

Polygon& PolygonSet::operator[](size_t index)
{
    return m_polygons.operator[](index);
}

Polygon PolygonSet::operator[](size_t index) const
{
    return m_polygons.operator[](index);
}

std::vector<QPointF> PolygonSet::points() const
{
    std::vector<QPointF> points;

    for (size_t i = 0; i < m_polygons.size(); ++i)
    {
        auto polygon = m_polygons[i].points();
        points.insert(points.end(), polygon.begin(), polygon.end());

        for (const auto& hole : m_polygons[i].holes())
        {
            points.insert(points.end(), hole.begin(), hole.end());
        }
    }

    return points;
}

std::vector<QLineF> PolygonSet::lines() const
{
    std::vector<QLineF> lines;

    auto extractLines = [&lines](const std::vector<QPointF>& points)
    {
        const size_t lastIndex = points.size() - 1;

        for (size_t i = 0; i < lastIndex; ++i)
        {
            lines.push_back({ points[i], points[i + 1] });
        }
        lines.push_back({ points[lastIndex], points[0] });
    };

    for (size_t i = 0; i < m_polygons.size(); ++i)
    {
        extractLines(m_polygons[i].points());

        for (const auto& hole : m_polygons[i].holes())
        {
            extractLines(hole);
        }
    }

    return lines;
}

std::vector<Polygon>::iterator PolygonSet::begin()
{
    return m_polygons.begin();
}

std::vector<Polygon>::iterator PolygonSet::end()
{
    return m_polygons.end();
}

QRectF PolygonSet::bounds() const
{
    QRectF result;
    for (const auto& p : m_polygons)
    {
        result = result.united(p.bounds());
    }
    return result;
}

Polygon PolygonSet::toPolygon() const
{
    if (m_polygons.empty())
        return Polygon();
    Polygon result = m_polygons[0];
    for (int i = 1; i < m_polygons.size(); ++i)
        result.unite(m_polygons[i]);
    return result;
}