#include "motion/structures/polygon.h"

#include "motion/algorithms/utils.h"

#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/minkowski_sum_2.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_with_holes_2.h>
#include <CGAL/Vector_2.h>

#include <QPainterPath>
#include <QPolygonF>
#include <QPointF>
#include <QLineF>



#include <boost/geometry/geometry.hpp>
#include <boost/polygon/polygon.hpp>

#include <functional>
#include <execution>

namespace Motion
{

typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
typedef Kernel::Point_2 Point_2;
typedef Kernel::Segment_2 Segment_2;
typedef CGAL::Polygon_with_holes_2<Kernel> Polygon_with_holes_2;
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

typedef CGAL::Polygon_2<Kernel> Polygon_2;
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

class Polygon::Impl 
{
public:
    Polygon_with_holes_2 m_polygon;
};

Polygon::Polygon() : m_pImpl(std::make_unique<Impl>()) {}

Polygon::~Polygon() = default;

Polygon& Polygon::operator=(const Polygon& polygon)
{
    m_pImpl = std::make_unique<Impl>(*polygon.m_pImpl);
    return *this;
}

Polygon::Polygon(const QPolygonF& polygon)
    : m_pImpl(std::make_unique<Impl>())
{
    try {
        auto& points = m_pImpl->m_polygon.outer_boundary();

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

Polygon::Polygon(std::unique_ptr<Impl>&& pImpl)
    : m_pImpl(std::move(pImpl)) {}

Polygon::Polygon(const Polygon& polygon)
    : m_pImpl(std::make_unique<Impl>(*polygon.m_pImpl)) {}

bool Polygon::unite(const Polygon& polygon)
{
    bool bRet = false;

    Polygon_with_holes_2 result;


    bRet = CGAL::join(m_pImpl->m_polygon, polygon.m_pImpl->m_polygon, result);


    if (bRet)
        m_pImpl = std::make_unique<Impl>(Impl{result});

    return bRet;
}

Polygon Polygon::united(const Polygon& polygon) const
{
    Polygon_with_holes_2 result;

    CGAL::join(m_pImpl->m_polygon, polygon.m_pImpl->m_polygon, result);

    return Polygon(std::make_unique<Impl>(Impl{result}));
}

PolygonSet Polygon::subtracted(const Polygon& polygon) const
{
    std::list<Polygon_with_holes_2> result;

    CGAL::difference(m_pImpl->m_polygon, polygon.m_pImpl->m_polygon,
                     std::back_inserter(result));

    PolygonSet s;
    for (const auto& r : result)
    {
        s.insert(Polygon(std::make_unique<Impl>(Impl{r})));
    }

    return s;
}

PolygonSet Polygon::intersected(const Polygon& polygon) const
{
    std::list<Polygon_with_holes_2> result;

    CGAL::intersection(m_pImpl->m_polygon, polygon.m_pImpl->m_polygon,
                       std::back_inserter(result));

    PolygonSet s;
    for (const auto& r : result)
    {
        s.insert(Polygon(std::make_unique<Impl>(Impl{r})));
    }

    return s;
}

Polygon Polygon::minkowskiSum(const Polygon& polygon) const
{
    const Polygon_with_holes_2& P = m_pImpl->m_polygon;
    const Polygon_with_holes_2& Q = polygon.m_pImpl->m_polygon;
    assert(P.outer_boundary().orientation() == CGAL::Sign::COUNTERCLOCKWISE);
    assert(Q.outer_boundary().orientation() == CGAL::Sign::COUNTERCLOCKWISE);
    
    return Polygon(std::make_unique<Impl>(Impl{CGAL::minkowski_sum_2(P, Q)}));
}

QPolygonF Polygon::toPolygon(bool ignoreHoles) const
{
    auto outer = m_pImpl->m_polygon.outer_boundary();
    QPolygonF result = toQPolygonF(outer.vertices_begin(), outer.vertices_end());

    if (!ignoreHoles)
    {
        for (auto it = m_pImpl->m_polygon.holes_begin(); it != m_pImpl->m_polygon.holes_end(); ++it)
        {
            result = result.subtracted(toQPolygonF(it->vertices_begin(), it->vertices_end()));
        }
    }

    return result;
}

QPainterPath Polygon::toPath(bool ignoreHoles) const
{
    QPainterPath path;
    auto outer = m_pImpl->m_polygon.outer_boundary();
    path.addPolygon(toQPolygonF(outer.vertices_begin(), outer.vertices_end()));
    path.closeSubpath();

    if (!ignoreHoles)
    {
        for (auto it = m_pImpl->m_polygon.holes_begin(); it != m_pImpl->m_polygon.holes_end(); ++it)
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

    auto outer = m_pImpl->m_polygon.outer_boundary();

    for (auto& point : outer)
    {
        result.push_back(toQPointF(point));
    }

    return result;
}

std::vector<std::vector<QPointF>> Polygon::holes() const
{
    std::vector<std::vector<QPointF>> holes;

    for (auto it = m_pImpl->m_polygon.holes_begin(); it != m_pImpl->m_polygon.holes_end(); ++it)
    {
        holes.push_back(std::vector<QPointF>());
        for (auto& point : *it)
        {
            holes.back().push_back(toQPointF(point));
        }
    }

    return holes;
}

QRectF Polygon::bounds() const
{
    return toQRectF(m_pImpl->m_polygon.bbox());
}

bool Polygon::inside(const QPointF& point, bool bStrict) const
{
    if (!insideRect(point, bounds()))
    {
        return false;
    }

    const Point_2 mpoint = Point_2(point.x(), point.y());

    auto outer = m_pImpl->m_polygon.outer_boundary();

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

    for (auto holesIter = m_pImpl->m_polygon.holes_begin(); 
          holesIter != m_pImpl->m_polygon.holes_end(); ++holesIter)
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

    return true;
}

bool Polygon::intersects(const QLineF& line, bool bStrict, std::vector<QPointF>* out) const
{
    boost::geometry::model::polygon<point_xy> polygon;

    auto outer = m_pImpl->m_polygon.outer_boundary();

    for (auto it = outer.begin(); it != outer.end(); ++it)
    {
        polygon.outer().push_back(point_xy(CGAL::to_double(it->x()), CGAL::to_double(it->y())));
    }

    polygon.outer().push_back(*polygon.outer().begin());

    for (auto IT = m_pImpl->m_polygon.holes_begin(); IT != m_pImpl->m_polygon.holes_end(); ++IT)
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
        if (m_pImpl->m_polygon.has_holes())
        {
            return false;
        }
        else
        {
            return m_pImpl->m_polygon.outer_boundary().is_simple();
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

}  // namespace Motion