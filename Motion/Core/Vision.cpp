#include "Vision.h"

#include "Core/DisplayView.h"
#include "Core/Algorithms/ConvexHull.h"

#include "QGraphicsEllipseItem"
#include "QGraphicsRectItem"
#include "QGraphicsPolygonItem"
#include "QPainter"
#include "QVector"

#include <functional>
#include <execution>
#include <mutex>

#include "Core/Algorithms/Utils.h"

namespace
{

int D = 750;

Polygon getShadow1(const QPolygonF& polygon, QPointF pos)
{
    std::vector<QPointF> result = polygon.toList().toVector().toStdVector();
    for (const QPointF& p : polygon)
    {
        QPointF newP = (QVector2D(p) + QVector2D(p - pos).normalized() * 5000).toPoint();
        result.push_back(newP);
    }
    return Polygon(unclose(convexHull(result)));
}

Polygon getShadow2(const QPolygonF& polygon, QPointF pos)
{

    const auto points1 = polygon.toList().toVector().toStdVector();
    std::vector<QPointF> points2;
    for (const QPointF& p : polygon)
    {
        QPointF newP = (QVector2D(p) + QVector2D(p - pos).normalized() * 5000).toPoint();
        points2.push_back(newP);
    }

    Polygon result(polygon);
    for (int i = 0; i < points1.size() - 1; ++i)
    {
        if (points1[i] == points1[i + 1])
            continue;
        auto p = QPolygonF({ points1[i], points2[i], points2[i + 1], points1[i + 1] });
        result = result.united(Polygon(p));
    }


    return result;
}

Polygon getShadow(const QPolygonF& polygon, QPointF pos)
{
    if (!isConvex(polygon))
        return getShadow2(polygon, pos);
    return getShadow1(polygon, pos);
}

QPolygonF getEllipse(QPointF center, int radius, int n = 30)
{
    QPolygonF result;
    double theta = 3.14159265358979323846 * 2 / n;
    for (int i = 0; i < n; ++i)
    {
        double newX = sin(theta * i) * radius + center.x();
        double newY = cos(theta * i) * radius + center.y();
        result.push_back(QPointF(newX, newY));
    }
    return result;
}

}

class QVision : public QGraphicsItem
{
public:
    QVision(Vision* pVision) : m_pVision(pVision) {}

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
    {
        //painter->drawPolygon(m_pVision->m_vision);
        painter->setBrush(QBrush(QColor(0, 0, 0, 180), Qt::BrushStyle::SolidPattern));
        painter->setPen(QPen(Qt::NoPen));
        for (const auto& p : m_pVision->m_dark)
            painter->drawPolygon(p);

        painter->setBrush(QBrush(QColor(0, 0, 0, 0), Qt::BrushStyle::SolidPattern));
        painter->setPen(QPen(Qt::green, 10));
        for (const QPointF& p : m_pVision->m_known)
            painter->drawPoint(p);

        painter->setPen(QPen(Qt::red, 10));
        for (const QPointF& p : m_pVision->m_unknown)
            painter->drawPoint(p);
    }

    QRectF boundingRect() const
    {
        return getSceneBBox();
    }
private:
    Vision *m_pVision;
};

Vision::Vision(QGraphicsItemGroup* pGroup) :
    m_pGroup(pGroup)
{
    reset();
}

Vision::~Vision()
{
    //if (m_pDrawPath)
    //    delete m_pDrawPath;
}

void Vision::setEnabled(bool bEnabled)
{
    m_bEnabled = bEnabled;
    if (bEnabled)
        reset();
}

bool Vision::getEnabled()
{
    return m_bEnabled;
}

void Vision::reset(std::optional<QPointF> pos)
{
    if (m_pDrawPath)
    {
        m_pGroup->removeFromGroup(m_pDrawPath);
        delete m_pDrawPath;
        m_pDrawPath = nullptr;
    }
    m_obstacles = {};
    m_dark = {};
    m_currentVision = {};
    m_visionHistory = {};
    m_known = {};
    m_unknown = {};
    m_vision = {};
    m_pDrawPath = new QVision(this);
    m_pGroup->addToGroup(m_pDrawPath);
    m_scene = Polygon(unclose(QPolygonF(getSceneBBox(1.75))));
    m_obss = {};
    if (!pos)
    {
        DisplayView* pDisplayView = DisplayView::getInstance();
        pos = pDisplayView->getDevicePosition();
    }
    update(*pos);
    update(*pos); // by design (fixes bug)
    calculateBorder();
}

void Vision::update(QPointF pos)
{
    if (!m_bEnabled)
        return;

    m_pos = pos;

    if (m_obss.size() == 0)
    {
        DisplayView* pDisplayView = DisplayView::getInstance();
        m_obstacles = pDisplayView->getObstacles();
        for (const auto& obs : m_obstacles)
            m_obss.insert(Polygon(obs, true));
    }


    m_currentVision = Polygon(unclose(getEllipse(m_pos, D / 2)));

    //std::vector<QPointF> newUnknown;
    //for (const auto& p : m_unknown)
    //{
    //    if (euclideanDist(p, m_pos) > 100)
    //        newUnknown.push_back(p);
    //}
    //m_unknown = newUnknown;

    for (int i = 0; i < m_obstacles.size(); ++i)
    {
        const auto& obstacle = m_obstacles[i];
        Polygon shadow = getShadow(obstacle, pos);
        if (m_currentVision.bounds().intersects(shadow.bounds()))
        {
            m_currentVision = m_currentVision.subtracted(shadow).toPolygon();
        }
    }

    if (m_visionHistory.points().empty())
        m_visionHistory = m_currentVision;
    else
        m_visionHistory.unite(m_currentVision);

    calculateDark();
    m_pDrawPath->update();
}

void Vision::calculateBorder()
{
    if (!m_bEnabled)
        return;

    m_known = {};
    m_unknown = {};

    //for (const auto& p : m_visionHistory.points())
    //{
    //    if (getSceneBBox().contains(p))
    //        m_unknown.push_back(p);
    //}

    //return;

    

    DisplayView* pDisplayView = DisplayView::getInstance();
    auto mSums = pDisplayView->getObstaclesMSums();

    QPolygonF vision = m_visionHistory.toPolygon();
    
    std::mutex mtx;
    bool skipKnown = false;
    auto func =
        [&](auto&& vision)
    {
        QPointF p[3] = {
        (QVector2D(vision) + QVector2D(1, 1)).toPointF(),
        (QVector2D(vision) + QVector2D(-1, 1)).toPointF(),
        (QVector2D(vision) + QVector2D(0, -1)).toPointF(),
        };
        bool inside = false;
        if (!inside)
            inside = m_obss.intersects(QLineF(p[0], p[1]));
        if (!inside)
            inside = m_obss.intersects(QLineF(p[1], p[2]));
        if (!inside)
            inside = m_obss.intersects(QLineF(p[2], p[0]));
        mtx.lock();
        if (getSceneBBox().contains(vision))
            if (inside)
            {
                if (!skipKnown)
                    m_known.push_back(vision);
            }
            else
            {
                //m_unknown.push_back(vision);
                Polygon outPol;
                //m_known.push_back(vision);
                if (mSums.inside(vision, true))
                {
                    //std::vector<QPointF> outP;
                    //QLineF line(pDisplayView->getDevicePosition(), vision);
                    //outPol.intersects(line, false, &outP);
                    //for (const auto& p : outP)
                    //{
                    //    auto shifted = affineLine2(pDisplayView->getDevicePosition(), p, 50);
                    //    if (m_currentVision.inside(shifted) && !mSums.inside(shifted))
                    //        m_unknown.push_back(shifted);
                    //}
                    ////m_unknown.insert(m_unknown.begin(), outP.begin(), outP.end());
                    ////m_unknown.push_back();
                }
                else
                {
                    m_unknown.push_back(vision);
                }
            }
        mtx.unlock();
    };

    std::for_each(
        std::execution::par,
        vision.begin(),
        vision.end(),
        func);


    //std::vector<QPointF> pts;
    //for (int i = 0; i < vision.size(); ++i)
    //{
    //    int i1 = i;
    //    int i2 = i + 1;
    //    if (i == vision.size() - 1)
    //    {
    //        i2 = 0;
    //    }
    //    //pts.push_back(m_known[i1]);
    //    auto d = euclideanDist(vision[i1], vision[i2]);
    //    //double k = d / std::round(d / 50);
    //    //for (int i = 0; i <= std::trunc(d / k); ++i)
    //    //    pts.push_back(affineLine(vision[i1], vision[i2], i * k / d));
    //    if (d > 50)
    //    {
    //        pts.push_back(affineLine(vision[i1], vision[i2], 0.25));
    //        pts.push_back(affineLine(vision[i1], vision[i2], 0.5));
    //        pts.push_back(affineLine(vision[i1], vision[i2], 0.75));
    //    }
    //}
    //skipKnown = true;

    //std::for_each(
    //    std::execution::par,
    //    pts.begin(),
    //    pts.end(),
    //    func);
}

void Vision::calculateDark()
{
    m_vision = m_currentVision.toPolygon();
    QPolygonF vision = m_visionHistory.toPolygon();
    auto sub = m_scene.subtracted(m_visionHistory);
    m_dark = {};
    for (const auto& s : sub)
        m_dark.push_back(s.toPolygon());
}