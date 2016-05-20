#ifndef WImports_H
#define WImports_H

#include "../Models/MImports.h"

#include <QWidget>
#include <QTreeView>
#include <QItemSelection>


class WImports : public QWidget
{
    Q_OBJECT

public:
    WImports(QWidget *parent=0);



public slots:
    void clear();
    void refresh();
signals:
    void selectionChanged (const QItemSelection &selected);


private:
    void setupContent();


    MImports *model;
    QTreeView *view;

};

#endif // WImports_H
