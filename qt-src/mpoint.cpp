#include "mpoint.h"

MPoint::MPoint(QObject *parent) :
    QObject(parent),
    mx(0),
    my(0)
{

}

MPoint::MPoint(double x, double y, QObject *parent) :
    QObject(parent),
    mx(x),
    my(y)
{
}

MPoint::MPoint(const MPoint &other) :
    QObject(other.parent()),
    mx(other.mx),
    my(other.my)
{
}

MPoint::MPoint(const QPointF &p) :
    QObject(),
    mx(p.x()), my(p.y())
{
}

void MPoint::setX(double newX)
{
    if (newX != mx) {
        mx = newX;
        emit xChanged();
    }
}

void MPoint::setY(double newY)
{
    if (newY != my) {
        my = newY;
        emit yChanged();
    }
}

bool operator!=(const MPoint& lhs, const MPoint& rhs) {
    return !(lhs.x() == rhs.x() && lhs.y() == rhs.y());
}

QDebug operator<<(QDebug debug, const MPoint &c)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << '(' << c.x() << ", " << c.y() << ')';
    return debug;
}


