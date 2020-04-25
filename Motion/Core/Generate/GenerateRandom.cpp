#include "GenerateRandom.h"
#include "Core/DisplayView.h"

void GenerateRandom::generate()
{
    DisplayView* pDisplayView = DisplayView::getInstance();
    auto device = pDisplayView->getDevicePolygon();

    int n = rand() % 30 + 20;
    
    const int w = DisplayView::WIDTH * 0.75;
    const int h = DisplayView::HEIGHT * 0.75;

    for (int i = 0; i < n; ++i)
    {
        qreal x = rand() % w - w / 2;
        qreal y = rand() % h - h / 2;
        qreal size = rand() % 400 + 200;
        QRectF rect = QRectF(QPointF(x - size / 2, y - size / 2), QSizeF(size, size));
        auto polygon = generateRandomShape(rect);
        if (device.intersects(polygon))
        {
            i--;
            continue;
        }
        m_pDisplayView->addObstacle(polygon);
    }
}
