#pragma once

#include "Core/States/IState.h"

#include "QGraphicsView"
#include "QGraphicsPolygonItem"
#include "QGraphicsItemGroup"
#include "QMouseEvent"

#include "Core/DeviceGraphicsItem.h"
#include "Core/ConfigurationSpace.h"
#include "Core/Structures/Polygon.h"
#include "Core/Algorithms/FindMethods/IFindMethod.h"

#include <memory>

class DisplayView : public QGraphicsView
{
    Q_OBJECT

public:
    friend class IdleState;
    friend class CreateState;
    friend class ReshapeState;

    const static int WIDTH = 3000;
    const static int HEIGHT = 3000;

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

    void wheelEvent(QWheelEvent* pWheelEvent);
    void mousePressEvent(QMouseEvent* pWheelEvent);
    void mouseReleaseEvent(QMouseEvent* pWheelEvent);

private:
    void clearGroup(QGraphicsItemGroup* group);

private:
    static DisplayView* m_pInstance;
    std::unique_ptr<IState> m_pState;
    std::unique_ptr<IFindMethod> m_pFindMethod;

    QGraphicsScene* m_pScene;

    QGraphicsItemGroup* m_pPolygonPreviewGroup;
    QGraphicsItemGroup* m_pObstaclesGroup;
    QGraphicsItemGroup* m_pMinkowskiGroup;
    QGraphicsItemGroup* m_pPathMapGroup;

    QGraphicsTextItem* m_pPathInfo;

    DeviceGraphicsItem* m_pDevice;
    ConfigurationSpace m_configurationSpace;
};