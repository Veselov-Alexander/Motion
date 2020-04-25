#pragma once

#include "Core/DisplayView.h"
#include "QRectF"
#include <CGAL/convex_hull_2.h>
#include <CGAL/Convex_hull_traits_adapter_2.h>

typedef CGAL::Convex_hull_traits_adapter_2<Kernel, CGAL::Pointer_property_map<Point_2>::type > Convex_hull_traits_2;

class Generate
{
public:
    void initialize(DisplayView* pDisplayView)
    {
        m_pDisplayView = pDisplayView;
    }
    virtual void generate() = 0;
protected:
    QPolygonF generateRandomShape(const QRectF& rect)
    {
        const int w = int(rect.width());
        const int h = int(rect.height());

        std::vector<Point_2> points;

        const int n = rand() % 10 + 3;

        for (int i = 0; i < n; ++i)
        {
            const QPointF point = rect.topLeft() + QPointF(rand() % w, rand() % h);
            points.push_back(Point_2(point.x(), point.y()));
        }
        std::vector<std::size_t> indices(points.size()), out;
        std::iota(indices.begin(), indices.end(), 0);

        CGAL::convex_hull_2(indices.begin(), indices.end(), std::back_inserter(out),
            Convex_hull_traits_2(CGAL::make_property_map(points)));
        QPolygonF output;
        for (std::size_t i : out) 
        {
            output.append(QPointF(CGAL::to_double(points[i].x()), CGAL::to_double(points[i].y())));
        }
        return output;
    }
protected:
    DisplayView* m_pDisplayView;
};