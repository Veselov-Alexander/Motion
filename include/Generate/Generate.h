#pragma once

#include "display_view.h"
#include "QRectF"
#include "algorithms/convex_hull.h"

class Generate
{
public:
    void initialize(DisplayView* pDisplayView)
    {
        m_pDisplayView = pDisplayView;
        m_bUseSensors = m_pDisplayView->getVision()->getEnabled();
        m_pDisplayView->useSensors(false);
    }

    void generate()
    {
        generateInternal();
        m_pDisplayView->useSensors(m_bUseSensors);
    }

    virtual void generateInternal() = 0;
protected:
    QPolygonF generateRandomShape(const QRectF& rect)
    {
        const int w = int(rect.width());
        const int h = int(rect.height());

        std::vector<QPointF> points;

        const int n = rand() % 10 + 3;

        for (int i = 0; i < n; ++i)
        {
            const QPointF point = rect.topLeft() + QPointF(rand() % w, rand() % h);
            points.push_back(point);
        }
        
        return convexHull(points);
    }
protected:
    DisplayView* m_pDisplayView;
    bool m_bUseSensors = false;
};