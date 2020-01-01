#ifndef MPOINT_H
#define MPOINT_H

#include <QObject>
#include <QDebug>
#include <QDebugStateSaver>
#include <QPointF>

// QQmlListProperty can only expose pointers to QObjects
// and QPoint is not a QObject (!!!)
class MPoint : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double x READ x WRITE setX NOTIFY xChanged)
    Q_PROPERTY(double y READ y WRITE setY NOTIFY yChanged)
public:
    explicit MPoint(QObject *parent = nullptr);
    explicit MPoint(double x, double y, QObject *parent = nullptr);
    MPoint(const MPoint& other);
    MPoint(const QPointF& p);
    MPoint operator-(const MPoint& other) { return MPoint(mx - other.mx, my - other.my); }
    MPoint operator+(const MPoint& other) { return MPoint(mx + other.mx, my + other.my); }
signals:
    void xChanged();
    void yChanged();

public slots:
    double x() const { return mx; }
    double y() const { return my; }
    void setX(double newX);
    void setY(double newY);

private:
    double mx;
    double my;
};

bool operator!=(const MPoint& lhs, const MPoint& rhs);
QDebug operator<<(QDebug debug, const MPoint &c);

#endif // MPOINT_H
