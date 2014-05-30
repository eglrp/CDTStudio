#include "cdtproject.h"
#include "dialognewimage.h"
#include <QInputDialog>
#include <QMenu>


CDTProject::CDTProject(QUuid uuid, QObject *parent):
    CDTBaseObject(uuid,parent),
    actionAddImage(new QAction(tr("Add Image"),this)),
    removeAllImages(new QAction(tr("Remove All images"),this)),
    actionRename(new QAction(tr("Rename Project"),this))
{
    removeAllImages->setIcon(QIcon(":/Icon/remove.png"));
    actionAddImage->setIcon(QIcon(":/Icon/add.png"));
    actionRename->setIcon(QIcon(":/Icon/rename.png"));

    keyItem=new CDTProjectTreeItem(CDTProjectTreeItem::PROJECT_ROOT,CDTProjectTreeItem::GROUP,QString(),this);
    valueItem=new CDTProjectTreeItem(CDTProjectTreeItem::VALUE,CDTProjectTreeItem::EMPTY,QString(),this);
    connect(actionAddImage,SIGNAL(triggered()),this,SLOT(addImageLayer()));
    connect(removeAllImages,SIGNAL(triggered()),this,SLOT(removeAllImageLayers()));
    connect(actionRename,SIGNAL(triggered()),this,SLOT(onActionRename()));
}

CDTProject::~CDTProject()
{
    if (id().isNull())
        return;

    QSqlQuery query(QSqlDatabase::database("category"));
    bool ret;
    ret = query.exec("delete from project where id = '"+uuid.toString()+"'");
    if (!ret)
        qWarning()<<"prepare:"<<query.lastError().text();
}

void CDTProject::addImageLayer()
{
    DialogNewImage dlg;
    if(dlg.exec() == DialogNewImage::Accepted)
    {
        CDTImageLayer *image = new CDTImageLayer(QUuid::createUuid(),this);
        image->setNameAndPath(dlg.imageName(),dlg.imagePath());
        keyItem->appendRow(image->standardItems());
        addImageLayer(image);
    }
}

void CDTProject::removeImageLayer(CDTImageLayer* image)
{
    int index = images.indexOf(image);
    if (index>=0)
    {
        image->removeAllSegmentationLayers();
        QStandardItem* keyItem = image->standardItems()[0];
        keyItem->parent()->removeRow(keyItem->index().row());
        images.remove(index);
        emit removeLayer(QList<QgsMapLayer*>()<<image->canvasLayer());
        delete image;
        emit projectChanged();
    }
}

void CDTProject::removeAllImageLayers()
{    
    foreach (CDTImageLayer* image, images) {
        removeImageLayer(image);
    }
}

void CDTProject::addImageLayer(CDTImageLayer *image)
{
    images.push_back(image);
    emit projectChanged();
}

void CDTProject::insertToTable(QString name)
{
    QSqlQuery query(QSqlDatabase::database("category"));
    query.prepare("insert into project values(?,?)");
    qDebug()<<id();
    qDebug()<<id();
    query.bindValue(0,id().toString());
    query.bindValue(1,name);
    query.exec();
    setName(name);
}

QString CDTProject::name() const
{
    QSqlQuery query(QSqlDatabase::database("category"));
    query.prepare("select name from project where id = ?");
    query.bindValue(0,id().toString());
    query.exec();
    query.next();
    return query.value(0).toString();

//    return projectName;
}

void CDTProject::setName(const QString &name)
{
//    projectName = n;

    QSqlQuery query(QSqlDatabase::database("category"));
    query.prepare("UPDATE project set name = ? where id =?");
    query.bindValue(0,name);
    query.bindValue(1,this->id().toString());
    query.exec();

    keyItem->setText(name);
    emit projectChanged();
}

void CDTProject::onContextMenuRequest(QWidget* parent)
{    
    QMenu* menu =new QMenu(parent);
    menu->addAction(actionAddImage);
    menu->addAction(removeAllImages);
    menu->addSeparator();
    menu->addAction(actionRename);

    menu->exec(QCursor::pos());
}

void CDTProject::onActionRename()
{
    bool ok;
    QString text = QInputDialog::getText(NULL, tr("Input Project Name"),
                                         tr("Project name:"), QLineEdit::Normal,
                                         name(), &ok);
    if (ok && !text.isEmpty())
    {
        setName(text);
    }
}

QDataStream &operator <<(QDataStream &out,const CDTProject &project)
{
    out<<project.id()<<project.name();
    out<<project.images.size();
    foreach (CDTImageLayer* image, project.images) {
        out<<*image;
    }

    return out;
}

QDataStream &operator >>(QDataStream &in, CDTProject &project)
{
    QString name;
    in>>project.uuid>>name;
    project.setName(name);

    project.insertToTable(name);

    int count;
    in>>count;
    for (int i=0;i<count;++i)
    {
        CDTImageLayer* image = new CDTImageLayer(QUuid(),&project);
        in>>*image;
        project.keyItem->appendRow(image->standardItems());
        project.images.push_back(image);
    }    
    return in;
}
