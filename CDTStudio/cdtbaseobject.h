#ifndef CDTBASEOBJECT_H
#define CDTBASEOBJECT_H

#include <QObject>
class CDTProjectTreeItem;
class QStandardItem;
class QgsMapLayer;

class CDTBaseObject : public QObject
{
    Q_OBJECT
public:
    explicit CDTBaseObject(QObject *parent = 0);

    virtual void onContextMenuRequest(QWidget *parent) = 0;
    QList<QStandardItem *> standardItems()const;
signals:
    void appendLayer(QList<QgsMapLayer*> layer);
    void removeLayer(QList<QgsMapLayer*> layer);
public slots:

protected:
    CDTProjectTreeItem* keyItem;
    CDTProjectTreeItem* valueItem;
    QgsMapLayer * mapCanvasLayer;
};

#endif // CDTBASEOBJECT_H
