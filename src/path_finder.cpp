#include "path_finder.h"

#include "display_view.h"
#include "algorithms/Utils.h"

#include "QMessageBox"

namespace
{

double measureTime(std::function<void(void)> f)
{
    auto start = std::chrono::system_clock::now();
    f();
    auto end = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / 1000.0;
}

QString getPathInfo(const Path& path, double elapsed = 0.0)
{
    qreal length = pathLength(path);
    QString pathInfo;
    pathInfo.sprintf("Path length: %4.2f units", length);
    if (!qIsNull(elapsed))
    {
        QString timeInfo;
        timeInfo.sprintf("\nCalculation time: %6.4f s.", elapsed);
        pathInfo += timeInfo;
    }
    return pathInfo;
}

void showInvalidDestinationMessageBox(DisplayView* pDisplayView)
{
    QMessageBox::warning(
        pDisplayView->parentWidget(),
        "Invalid destination",
        "Destination point is inside the polygon."
    );
}

void showNoPathFoundMessageBox(DisplayView* pDisplayView)
{
    QMessageBox::warning(
        pDisplayView->parentWidget(),
        "Could not find a path",
        "No path exists or was not found."
    );
}

void showOutOfBoundsMessageBox(DisplayView* pDisplayView)
{
    QMessageBox::warning(
        pDisplayView->parentWidget(),
        "Out of bounds",
        "You clicked out of scene."
    );
}

}

void IPathFinder::displayPath(const Path& path)
{
    DisplayView* pDisplayView = DisplayView::getInstance();
    QPainterPath qpath;

    if (!path.empty())
    for (size_t i = 0; i < path.size() - 1; ++i)
    {
        QPainterPath pathLine(path[i]);
        pathLine.lineTo(path[i + 1]);

        qpath.addPath(pathLine);
    }

    auto gpath = new QGraphicsPathItem(qpath);
    gpath->setPen(QPen(Qt::blue, 3));

    pDisplayView->clearGroup(pDisplayView->m_pPathMapGroup);
    pDisplayView->m_pPathMapGroup->addToGroup(pDisplayView->getFindMethod()->getPathMap());
    pDisplayView->m_pPathMapGroup->addToGroup(gpath);
}

void RegularPathFinder::findPath(QPoint physicCoord)
{
    DisplayView* pDisplayView = DisplayView::getInstance();

    QPointF source = pDisplayView->getDevicePosition();
    QPointF destination = pDisplayView->mapToScene(physicCoord);
    if (!getSceneBBox().contains(destination))
    {
        showOutOfBoundsMessageBox(pDisplayView);
        return;
    }
    PolygonSet obstacles = pDisplayView->getObstaclesMSums();

    if (obstacles.inside(destination))
    {
        showInvalidDestinationMessageBox(pDisplayView);
        return;
    }

    IFindMethod* pFindMethod = pDisplayView->getFindMethod();
    assert(pFindMethod);

    Path path;

    double elapsed = measureTime([&]() {
        path = pFindMethod->findPath(source, destination, obstacles);
    });
    
    if (path.empty())
    {
        showNoPathFoundMessageBox(pDisplayView);
        pDisplayView->clearGroup(pDisplayView->m_pPathMapGroup);
        pDisplayView->m_pPathMapGroup->addToGroup(pFindMethod->getPathMap());
        return;
    }

    pDisplayView->setPathInfo(getPathInfo(path, elapsed), destination);
    pDisplayView->moveDevice(path);
    displayPath(path);
}

void VisionPathFinder::findPath(QPoint physicCoord)
{
    DisplayView* pDisplayView = DisplayView::getInstance();
    Vision* vision = pDisplayView->getVision();

    m_source = pDisplayView->getDevicePosition();
    m_destination = pDisplayView->mapToScene(physicCoord);
    if (!getSceneBBox().contains(m_destination))
    {
        showOutOfBoundsMessageBox(pDisplayView);
        return;
    }
    m_obstacles = pDisplayView->getObstaclesMSums();
    m_pathFound = false;
    m_path = {};
    displayPath(m_path);
    m_visited.clear();

    if (m_obstacles.inside(m_destination))
    {
        showInvalidDestinationMessageBox(pDisplayView);
        return;
    }

    subPathReached();
}

void VisionPathFinder::subPathReached()
{
    DisplayView* pDisplayView = DisplayView::getInstance();
    Vision* vision = pDisplayView->getVision();

    vision->calculateBorder();

    m_source = pDisplayView->getDevicePosition();

    if (m_pathFound)
    {
        pDisplayView->setPathInfo(getPathInfo(m_path), m_destination);
        displayPath(m_path);
        return;
    }
    
    IFindMethod* pFindMethod = pDisplayView->getFindMethod();
    assert(pFindMethod);

    auto obstacles = m_obstacles;
    //Polygon scene = Polygon(unclose(QPolygonF(getSceneBBox())));
    //for (const auto& p : scene.subtracted(vision->get()))
    //    obstacles.insert(p);

    if (vision->getVisionHistory().inside(m_destination))
    {
        pFindMethod->setMask(vision->getVisionHistory());
        Path path = pFindMethod->findPath(m_source, m_destination, obstacles);
        m_path.insert(m_path.end(), path.begin(), path.end());
        if (!path.empty() && path != INVALID_PATH)
        {
            m_pathFound = true;
            m_visited.push_back(m_destination);
            displayPath(m_path);
            pDisplayView->moveDevice(path);
            return;
        }
    }

    auto equals = [](double d1, double d2, double eps)
    {
        return std::fabs(d1 - d2) < eps;
    };

    auto points = vision->getUnknownPoints();
    auto pos = pDisplayView->getDevicePosition();
    std::sort(points.begin(), points.end(), [&](const QPointF& p1, const QPointF& p2) {
        auto d1 = euclideanDistSqrd(p1, pos);
        auto d2 = euclideanDistSqrd(p2, pos);
        auto d3 = euclideanDistSqrd(p1, m_destination);
        auto d4 = euclideanDistSqrd(p2, m_destination);
        //auto v1 = visited(p1);
        //auto v2 = visited(p2);
        //if (v1 != v2)
        //    return v1 < v2;
        return d1 * 0.25 + d3  < d2 * 0.25 + d4 ;
    });
    for (const auto& point : points)
    {
        Polygon polygon;
        bool valid = true;
        QPointF shiftedPoint = point;
        /*if (m_obstacles.inside(point, true, &polygon))
        {
            valid = false;
            auto pts = polygon.points();
            for (int i = 0; i < pts.size(); ++i)
            {
                if (vision->getVisionHistory().inside(pts[i]))
                {
                    if (auto pp = shifted(i, pts, polygon))
                    {
                        if (!valid || euclideanDist(*pp, point) < euclideanDist(shiftedPoint, point))
                            shiftedPoint = *pp;
                        valid = true;
                    }
                }
            }
        }
        if (!valid)
            continue;*/

        if (visited(shiftedPoint))
            continue;

        pFindMethod->setMask(vision->getVisionHistory());
        Path subPath = pFindMethod->findPath(m_source, shiftedPoint, obstacles);
        if (!subPath.empty() && subPath != INVALID_PATH && pathLength(subPath) > 0.0001)
        {
            m_path.insert(m_path.end(), subPath.begin(), subPath.end());
            m_visited.insert(m_visited.end(), m_path.begin(), m_path.end());
            displayPath(m_path);
            pDisplayView->moveDevice(subPath);
            return;
        }
        else
        {
            m_visited.push_back(shiftedPoint);
        }
    }
    showNoPathFoundMessageBox(pDisplayView);
}

bool VisionPathFinder::visited(const QPointF& v)
{
    return std::find_if(m_visited.begin(), m_visited.end(), [&v](const QPointF& r)
        {
            double eps = 10;
            return std::fabs(v.x() - r.x()) < eps &&
                std::fabs(v.y() - r.y()) < eps;
        }) != m_visited.end();
}