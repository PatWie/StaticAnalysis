#ifndef WSegments_H
#define WSegments_H

#include "../Models/MSegments.h"

#include <QWidget>
#include <QTreeView>
#include <QItemSelection>


class WSegments : public QWidget
{
    Q_OBJECT

public:
    WSegments(QWidget *parent=0);

public slots:

    void refresh();
signals:
    void selectionChanged (const QItemSelection &selected);


private slots:

private:
    void setupContent();


    MSegments *model;
    QTreeView *view;

};

#endif // WSegments_H
