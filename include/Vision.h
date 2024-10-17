#pragma once

#include "QGraphicsItemGroup"
#include "structures/polygon.h"

#include <optional>

class QVision;
typedef std::vector<QPolygonF> QPolygonFList;

class Vision
{
public:
    friend class QVision;

    Vision(QGraphicsItemGroup* pGroup);
    ~Vision();
    void setEnabled(bool bEnabled);
    bool getEnabled();
    void reset(std::optional<QPointF> pos = std::nullopt);
    void update(QPointF pos);
    void calculateBorder();

    Polygon getVisionHistory() { return m_visionHistory; }
    std::vector<QPointF> getUnknownPoints() { return m_unknown; }
private:
    void calculateDark();

    QPointF m_pos;
    QGraphicsItemGroup* m_pGroup = nullptr;
    QVision* m_pDrawPath = nullptr;
    Polygon m_currentVision;
    Polygon m_visionHistory;
    Polygon m_scene;
    QPolygonF m_vision;
    QPolygonFList m_dark;
    QPolygonFList m_obstacles;
    std::vector<QPointF> m_known, m_unknown;
    PolygonSet m_obss;
    bool m_bEnabled = false;
};