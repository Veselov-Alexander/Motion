#include "DisplayView.h"

#include "DisplayView.h"
#include "States/IdleState.h"
#include "Structures/Polygon.h"
#include "Algorithms/Utils.h"

#include "QtMath"

const QPolygonF DisplayView::DEFAULT = 
QPolygonF({
    { 0, 50 },
    { 10, 10},
    { 50, 0 },
    { 10, -10 },
    { 0, -50 },
    { -10, -10 },
    { -50, 0 },
    { -10, 10 },
});

DisplayView* DisplayView::m_pInstance = nullptr;

bool bUseVision = true;

DisplayView::DisplayView(QWidget *parent) : QGraphicsView(parent)
{
    if (!m_pInstance)
    {
        m_pInstance = this;

        QAbstractScrollArea::setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        QAbstractScrollArea::setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        m_pScene = new QGraphicsScene();
        m_pScene->setSceneRect(-WIDTH / 2, -HEIGHT / 2, WIDTH, HEIGHT);

        m_pScene->addRect(m_pScene->sceneRect()); // bounds of scene

        m_pPolygonPreviewGroup = new QGraphicsItemGroup();
        m_pScene->addItem(m_pPolygonPreviewGroup);

        m_pObstaclesGroup = new QGraphicsItemGroup();
        m_pScene->addItem(m_pObstaclesGroup);

        m_pMinkowskiGroup = new QGraphicsItemGroup();
        m_pMinkowskiGroup->setVisible(false);
        m_pScene->addItem(m_pMinkowskiGroup);

        m_pPathMapGroup = new QGraphicsItemGroup();
        m_pPathMapGroup->setVisible(false);
        m_pScene->addItem(m_pPathMapGroup);

        m_pVisionGroup = new QGraphicsItemGroup();
        m_pScene->addItem(m_pVisionGroup);
        m_pVision = new Vision(m_pVisionGroup);
        useSensors(bUseVision);

        m_pDevice = new DeviceGraphicsItem(DEFAULT, m_pVision);
        m_configurationSpace = ConfigurationSpace();
        m_pScene->addItem(m_pDevice);

        initPathInfo();

        setScene(m_pScene);
        setState(new IdleState());
        setRenderHints(QPainter::Antialiasing);
    }
    else
    {
        throw "DisplayView already initialized";
    }
}

DisplayView::~DisplayView()
{
    m_pScene->clear();

    delete m_pVision;
    delete m_pScene;
}

DisplayView* DisplayView::getInstance()
{
    if (m_pInstance)
    {
        return m_pInstance;
    }
    assert("DisplayView instance deleted or not initialized.");
    return nullptr;
}

void DisplayView::reset(const QPolygonF& device)
{
    m_pDevice->stopAnimation();

    m_pScene->clear();

    m_pScene->addRect(m_pScene->sceneRect()); // bounds of scene

    m_pPolygonPreviewGroup = new QGraphicsItemGroup();
    m_pScene->addItem(m_pPolygonPreviewGroup);

    m_pObstaclesGroup = new QGraphicsItemGroup();
    m_pScene->addItem(m_pObstaclesGroup);

    m_pMinkowskiGroup = new QGraphicsItemGroup();
    m_pScene->addItem(m_pMinkowskiGroup);

    m_pPathMapGroup = new QGraphicsItemGroup();
    m_pScene->addItem(m_pPathMapGroup);

    m_pVisionGroup = new QGraphicsItemGroup();
    m_pScene->addItem(m_pVisionGroup);

    m_pDevice = nullptr;
    bool b = bUseVision;
    if (m_pVision)
    {
        b = m_pVision->getEnabled();
        delete m_pVision;
    }
    m_pVision = new Vision(m_pVisionGroup);

    m_pDevice = new DeviceGraphicsItem(device, m_pVision);
    m_pScene->addItem(m_pDevice);

    m_configurationSpace = ConfigurationSpace();

    initPathInfo();

    setState(new IdleState());
    useSensors(b);
}

void DisplayView::initPathInfo()
{
    m_pPathInfo = new QGraphicsTextItem();
    QFont font;
    font.setPointSizeF(15);
    m_pPathInfo->setFont(font);
    m_pPathInfo->setVisible(false);
    m_pScene->addItem(m_pPathInfo);
}

void DisplayView::clearGroup(QGraphicsItemGroup* group)
{
    if (group)
    {
        bool bVisible = group->isVisible();
        group->setVisible(true);
        for (QGraphicsItem* item : m_pScene->items(group->boundingRect()))
        {
            if (item->group() == group)
            {
                delete item;
            }
        }
        group->setVisible(bVisible);
    }
}

void DisplayView::setState(IState* state)
{
    m_pState.reset(state);
}

void DisplayView::setDevicePosition(const QPointF& position)
{
    m_pDevice->setDevicePosition(position);
}

void DisplayView::moveDevice(const Path& path)
{
    m_pDevice->move(path);
}

void DisplayView::reshapeDevice(const QPolygonF& polygon)
{
    m_pDevice->reshape(polygon);
    m_configurationSpace.update();
    m_pVision->reset();

    drawConfigurationSpace();
}

void DisplayView::setDeviceMovable(bool bEnabled)
{
    m_pDevice->setMovable(bEnabled);
}

IState* DisplayView::getState()
{
    return m_pState.get();
}

QPolygonF DisplayView::getDevicePolygon()
{
    return m_pDevice->polygon();
}

void DisplayView::undoDeviceMove()
{
    m_pDevice->undoMove();
}

void DisplayView::addObstacle(const QPolygonF& polygon)
{
    QGraphicsPolygonItem* pItem = m_configurationSpace.addObstacle(polygon);
    m_pObstaclesGroup->addToGroup(pItem);

    clearGroup(m_pPathMapGroup);
    drawConfigurationSpace();

    m_pVision->reset();
    m_pDevice->clearMovesHistory();
}

void DisplayView::drawConfigurationSpace()
{
    clearGroup(m_pMinkowskiGroup);
    m_configurationSpace.draw(m_pMinkowskiGroup);
}

void DisplayView::displayMinkowski(bool bDisplay)
{
    m_pMinkowskiGroup->setVisible(bDisplay);
}

void DisplayView::displayRoadmap(bool bDisplay)
{
    m_pPathMapGroup->setVisible(bDisplay);
}

void DisplayView::useSensors(bool bUse)
{
    m_pVisionGroup->setVisible(bUse);
    m_pVision->setEnabled(bUse);
    if (m_pVision->getEnabled())
        m_pPathFinder.reset(new VisionPathFinder());
    else
        m_pPathFinder.reset(new RegularPathFinder());
}

Vision* DisplayView::getVision()
{
    return m_pVision;
}

IPathFinder* DisplayView::getPathFinder()
{
    return m_pPathFinder.get();
}

QPointF DisplayView::getDevicePosition()
{
    if (m_pDevice)
        return m_pDevice->pos();
    return {};
}

QPolygonFList DisplayView::getObstacles()
{
    return m_configurationSpace.getObstacles();
}

PolygonSet DisplayView::getObstaclesMSums()
{
    return m_configurationSpace.getObstaclesMSums();
}

QRectF DisplayView::getViewRect()
{
    return m_pScene->sceneRect();
}

QString DisplayView::getPathInfo()
{
    return m_pPathInfo->toPlainText();
}

void DisplayView::setPathInfo(const QString& pathInfo, const QPointF& where)
{
    m_pPathInfo->setPlainText(pathInfo);
    QPointF shift(-m_pPathInfo->boundingRect().width() / 2.0, m_pDevice->boundingRect().height() / 4.0 * 3.0);
    m_pPathInfo->setPos(where + shift); 
}

void DisplayView::displayPathInfo(bool bDisplay)
{
    m_pPathInfo->setVisible(bDisplay);
}

void DisplayView::setFindMethod(IFindMethod* pFindMethod)
{
    m_pFindMethod.reset(pFindMethod);
}

IFindMethod* DisplayView::getFindMethod()
{
    return m_pFindMethod.get();
}

void DisplayView::wheelEvent(QWheelEvent* pWheelEvent)
{
    m_pState->wheelEvent(pWheelEvent);
}

void DisplayView::mousePressEvent(QMouseEvent* pWheelEvent)
{
    m_pState->mousePressEvent(pWheelEvent);
}

void DisplayView::mouseReleaseEvent(QMouseEvent* pWheelEvent)
{
    m_pState->mouseReleaseEvent(pWheelEvent);
}