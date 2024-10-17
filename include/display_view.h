#pragma once

#include "states/state.h"

#include "QGraphicsView"
#include "QGraphicsPolygonItem"
#include "QGraphicsItemGroup"
#include "QMouseEvent"

#include "vision.h"
#include "path_finder.h"
#include "device_graphics_item.h"
#include "configuration_space.h"
#include "structures/polygon.h"
#include "algorithms/find_methods/find_method.h"

#include <memory>

class DisplayView : public QGraphicsView
{
    Q_OBJECT

public:
    friend class IdleState;
    friend class CreateState;
    friend class ReshapeState;
    friend class IPathFinder;
    friend class VisionPathFinder;
    friend class RegularPathFinder;

    const static int HEIGHT = 2295;
    const static int WIDTH = DisplayView::HEIGHT * 2;

    const static QPolygonF DEFAULT;

    DisplayView(QWidget *pParent);
    ~DisplayView();

    DisplayView(DisplayView const&) = delete;
    DisplayView& operator= (DisplayView const&) = delete;

    static DisplayView* getInstance();

    void reset(const QPolygonF& device = DEFAULT);
    void initPathInfo();

    void setState(IState *pState);
    IState* getState();

    void setDevicePosition(const QPointF& position);
    QPointF getDevicePosition();
    void moveDevice(const Path& path);
    void reshapeDevice(const QPolygonF& polygon);
    void setDeviceMovable(bool bEnabled);
    QPolygonF getDevicePolygon();
    void undoDeviceMove();

    void addObstacle(const QPolygonF& polygon);
    QPolygonFList getObstacles();
    PolygonSet getObstaclesMSums();

    QRectF getViewRect();

    QString getPathInfo();
    void setPathInfo(const QString& pathInfo, const QPointF& where);
    void displayPathInfo(bool bDisplay);

    void setFindMethod(IFindMethod* pFindMethod);
    IFindMethod* getFindMethod();

    void drawConfigurationSpace();

    void displayMinkowski(bool bDisplay);
    void displayRoadmap(bool bDisplay);

    void useSensors(bool bUse);
    Vision* getVision();

    IPathFinder* getPathFinder();

    void wheelEvent(QWheelEvent* pWheelEvent);
    void mousePressEvent(QMouseEvent* pWheelEvent);
    void mouseReleaseEvent(QMouseEvent* pWheelEvent);

    void setUseSnapping(bool b)
    {
        m_bUseSnapping = b;
    }

    bool getUseSnapping()
    {
        return m_bUseSnapping;
    }

private:
    void clearGroup(QGraphicsItemGroup* group);
    void resetVision();

private:
    static DisplayView* m_pInstance;

    std::unique_ptr<IState> m_pState;
    std::unique_ptr<IFindMethod> m_pFindMethod;
    std::unique_ptr<IPathFinder> m_pPathFinder;

    ConfigurationSpace m_configurationSpace;

    QGraphicsScene* m_pScene = nullptr;

    QGraphicsItemGroup* m_pPolygonPreviewGroup = nullptr;
    QGraphicsItemGroup* m_pObstaclesGroup = nullptr;
    QGraphicsItemGroup* m_pMinkowskiGroup = nullptr;
    QGraphicsItemGroup* m_pPathMapGroup = nullptr;
    QGraphicsItemGroup* m_pVisionGroup = nullptr;

    QGraphicsTextItem* m_pPathInfo = nullptr;
    DeviceGraphicsItem* m_pDevice = nullptr;
    Vision* m_pVision = nullptr;

    bool m_bUseSnapping = false;
};