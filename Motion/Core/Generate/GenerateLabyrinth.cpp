#include "GenerateLabyrinth.h"

#include <random>

void GenerateLabyrinth::generate()
{
    const qreal fillCoef = 0.8;
    const qreal deviceScaleCoef = 1.3;

    QSizeF deviceSize = m_pDisplayView->getDevicePolygon().boundingRect().size();
    m_cellSize = std::max(deviceSize.width(), deviceSize.height()) * deviceScaleCoef;

    const int n = DisplayView::HEIGHT / m_cellSize * fillCoef;
    const int m = DisplayView::WIDTH / m_cellSize * fillCoef;

    const qreal height = n * m_cellSize;
    const qreal width = m * m_cellSize;

    m_topleft = QPointF(- (n / 2) * m_cellSize,
                        - (m / 2) * m_cellSize);

    m_visited.assign(n, std::vector<bool>(m, false));

    const int start = rand() % n;

    m_pDisplayView->setDevicePosition(m_topleft + QPointF((start + 0.5) * m_cellSize, 0.5 * m_cellSize));

    dfs(start, 0);

    display();
}

void GenerateLabyrinth::display()
{
    const qreal EPS = 1;

    for (int i = 0; i < m_visited.size(); ++i)
    {
        for (int j = 0; j < m_visited[0].size(); ++j)
        {
            if (!m_visited[i][j])
            {
                QPolygonF polygon;

                polygon.append(m_topleft + QPointF((i + 0) * m_cellSize - EPS, (j + 0) * m_cellSize - EPS));
                polygon.append(m_topleft + QPointF((i + 1) * m_cellSize + EPS, (j + 0) * m_cellSize - EPS));
                polygon.append(m_topleft + QPointF((i + 1) * m_cellSize + EPS, (j + 1) * m_cellSize + EPS));
                polygon.append(m_topleft + QPointF((i + 0) * m_cellSize - EPS, (j + 1) * m_cellSize + EPS));

                m_pDisplayView->addObstacle(polygon);
            }
        }
    }
}

bool GenerateLabyrinth::dfs(const int i, const int j)
{
    if (!inside(i, j) || m_visited[i][j] || neighbors(i, j) > 2)
        return false;

    m_visited[i][j] = true;

    std::vector<int> variants = { 0, 1, 2, 3 };

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine e(seed);

    std::shuffle(variants.begin(), variants.end(), e);

    for (auto variant : variants)
    {
        switch (variant)
        {

        case 0: // top
        {
            if (dfs(i + 1, j))
                return true;
        }

        case 1: // right
        {
            if (dfs(i, j + 1))
                return true;
        }

        case 2: // down
        {
            if (dfs(i - 1, j))
                return true;
        }

        case 3: // left
        {
            if (dfs(i, j - 1))
                return true;
        }

        }
    }

    return false;
}

int GenerateLabyrinth::neighbors(const int i, const int j)
{
    int count = 0;
    for (int dx = i - 1; dx <= i + 1; ++dx)
    {
        for (int dy = j - 1; dy <= j + 1; ++dy)
        {
            if (dx == i && dy == j || !inside(dx, dy))
                continue;

            count += m_visited[dx][dy];
        }
    }
    return count;
}

bool GenerateLabyrinth::inside(const int i, const int j)
{
    return !(i < 0 || j < 0 || i >= m_visited.size() || j >= m_visited[0].size());
}
