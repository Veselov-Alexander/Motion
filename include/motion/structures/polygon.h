#pragma once

#include <QRect>

#include <vector>
#include <memory>

class QPainterPath;
class QPolygonF;
class QPointF;
class QLineF;

namespace Motion
{

class PolygonSet;
class PolygonImpl;

class Polygon
{
public:
    Polygon();
    ~Polygon();
    Polygon(const Polygon& polygon);
    explicit Polygon(const QPolygonF& polygon);
    bool inside(const QPointF& point, bool bStrict = true) const;
    bool intersects(const QLineF& line, bool bStrict = false, std::vector<QPointF>* out = nullptr) const;
    bool isSimple() const;
    bool unite(const Polygon& polygon);
    Polygon united(const Polygon& polygon) const;
    PolygonSet intersected(const Polygon& polygon) const;
    PolygonSet subtracted(const Polygon& polygon) const;
    Polygon minkowskiSum(const Polygon& polygon) const;
    QPolygonF toPolygon(bool ignoreHoles = false) const;
    QPainterPath toPath(bool ignoreHoles = false) const;
    std::vector<QPointF> points() const;
    std::vector<std::vector<QPointF>> holes() const;
    QRectF bounds() const;
    Polygon& operator=(const Polygon& polygon);
private:
    class Impl;
    QRectF boundsRect(const QPolygonF& polygon);
    Polygon(std::unique_ptr<Impl>&& pImpl);
private:
    std::unique_ptr<Impl> m_pImpl;
    QRectF m_boundsRect;
};

class PolygonSet
{
public:
    void insert(const Polygon& polygon);
    bool inside(const QPointF& point, bool bStrict = true, Polygon* out = nullptr) const;
    bool intersects(const QLineF& line, bool bStrict = false) const;
    void clear();
    size_t size() const;
    Polygon& operator[](size_t index);
    Polygon operator[](size_t index) const;
    std::vector<QPointF> points() const;
    std::vector<QLineF> lines() const;
    std::vector<Polygon>::iterator begin();
    std::vector<Polygon>::iterator end();
    QRectF bounds() const;
    Polygon toPolygon() const;
    std::vector<Polygon> getPolygons() const
    {
        return m_polygons;
    }
private:
    std::vector<Polygon> m_polygons;
};

}  // namespace Motion