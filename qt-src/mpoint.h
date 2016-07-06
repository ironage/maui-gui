#ifndef MPOINT_H
#define MPOINT_H

#include <QObject>
#include <QDebug>
#include <QDebugStateSaver>

// QQmlListProperty can only expose pointers to QObjects
// and QPoint is not a QObject (!!!)
class MPoint : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int x READ x WRITE setX NOTIFY xChanged)
    Q_PROPERTY(int y READ y WRITE setY NOTIFY yChanged)
public:
    explicit MPoint(QObject *parent = 0);
    explicit MPoint(int x, int y, QObject *parent = 0);
    MPoint(const MPoint& other);
signals:
    void xChanged();
    void yChanged();

public slots:
    int x() const { return mx; }
    int y() const { return my; }
    void setX(int newX);
    void setY(int newY);

private:
    int mx;
    int my;
};

bool operator!=(const MPoint& lhs, const MPoint& rhs);
QDebug operator<<(QDebug debug, const MPoint &c);

#endif // MPOINT_H
