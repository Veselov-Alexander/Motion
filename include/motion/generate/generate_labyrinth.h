#pragma once

#include "Generate.h"

namespace Motion
{

class GenerateLabyrinth : public Generate
{
public:
    void generateInternal() override;
protected:
    virtual void display();
private:
    bool dfs(const int i, const int j);
    int neighbors(const int i, const int j);
    bool inside(const int i, const int j);
protected:
    QPointF m_topleft;
    qreal m_cellSize;
    std::vector<std::vector<bool>> m_visited;
};

}  // namespace Motion