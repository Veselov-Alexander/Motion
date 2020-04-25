#include "GeneratePolyLabyrinth.h"

void GeneratePolyLabyrinth::display()
{
    const qreal EPS = 1;

    for (int i = 0; i < m_visited.size(); ++i)
    {
        for (int j = 0; j < m_visited[0].size(); ++j)
        {
            if (!m_visited[i][j])
            {
                QPointF topleft = m_topleft + QPointF((i + 0) * m_cellSize - EPS, (j + 0) * m_cellSize - EPS);
                QPointF botright = m_topleft + QPointF((i + 1) * m_cellSize + EPS, (j + 1) * m_cellSize + EPS);

                m_pDisplayView->addObstacle(generateRandomShape(QRectF(topleft, botright)));
            }
        }
    }
}
