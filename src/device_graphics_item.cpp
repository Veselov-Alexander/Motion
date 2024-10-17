#include "motion/device_graphics_item.h"

#include <QBrush>s

#include "motion/algorithms/utils.h"
#include "motion/display_view.h"

namespace Motion
{

DeviceGraphicsItem::DeviceGraphicsItem(const QPolygonF& polygon, Vision* pVision) :
    QGraphicsPolygonItem(polygon),
    m_animationTimer(nullptr),
    m_animation(nullptr),
    m_pVision(pVision)
{
    reshape(polygon);

    QGraphicsPolygonItem::setBrush(QBrush(Qt::black, Qt::BrushStyle::SolidPattern));
}

void DeviceGraphicsItem::reshape(const QPolygonF& polygon)
{
    QPointF center = centroid(polygon);

    QGraphicsPolygonItem::setPos(center);
    QGraphicsPolygonItem::setPolygon(centered(polygon, {0, 0}));
}

class GraphicsItemAnimation : public QGraphicsItemAnimation
{
public:
    GraphicsItemAnimation(Vision* pVision) : QGraphicsItemAnimation()
    {
        m_pVision = pVision;
    }

    void afterAnimationStep(qreal step) override
    {
        m_pVision->update(posAt(step));
        if (step == 1.0)
        {
            DisplayView* pDisplayView = DisplayView::getInstance();
            pDisplayView->getPathFinder()->subPathReached();
        }
    }
private:
    Vision* m_pVision;
};

void DeviceGraphicsItem::move(const Path& path)
{
    int nSpeed = 2;
    int nTotalDuration = pathLength(path) * nSpeed;

    m_positions.push(QGraphicsPolygonItem::pos());
    
    m_animationTimer = new QTimeLine(nTotalDuration);
    m_animationTimer->setFrameRange(0, 300);
    m_animationTimer->setCurveShape(QTimeLine::CurveShape::LinearCurve);

    m_animation = new GraphicsItemAnimation(m_pVision);
    m_animation->setItem(this);
    m_animation->setTimeLine(m_animationTimer);

    qreal pos = 0;
    for (int i = 0; i < path.size() - 1; ++i)
    {
        int nCurrentDuration = QLineF(path[std::max(0, i - 1)], path[i]).length() * nSpeed;
        pos += static_cast<double>(nCurrentDuration) / nTotalDuration;
        m_animation->setPosAt(pos, path[i]);
    }
    m_animation->setPosAt(1.0, path.back());

    m_animationTimer->start();
}

bool DeviceGraphicsItem::intersects(const QPolygonF& polygon)
{
    return centered(this->polygon(), this->pos()).intersects(polygon);
}

void DeviceGraphicsItem::setDevicePosition(const QPointF& position)
{
    QGraphicsPolygonItem::setPos(position);
}

void DeviceGraphicsItem::setMovable(bool bEnabled)
{
    setFlag(QGraphicsItem::ItemIsMovable, bEnabled);
}

void DeviceGraphicsItem::stopAnimation()
{
    if (!m_animationTimer) return;

    if (m_animationTimer && m_animationTimer->state() == QTimeLine::State::Running)
    {
        m_animationTimer->stop();
    }
}

void DeviceGraphicsItem::undoMove()
{
    if (!m_positions.empty())
    {
        stopAnimation();
        setDevicePosition(m_positions.top());
        m_positions.pop();
    }
}

void DeviceGraphicsItem::clearMovesHistory()
{
    m_positions = {};
}

}  // namespace Motion