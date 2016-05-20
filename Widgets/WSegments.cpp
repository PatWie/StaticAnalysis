#include "WSegments.h"
#include <QGridLayout>
#include <QTreeView>
#include <QMap>

#include "../tia.h"

WSegments::WSegments(QWidget *parent) :
    QWidget(parent)
{
    QGridLayout *layout = new QGridLayout(this);
    view = new QTreeView ;
    view->setIndentation(0);
    layout->addWidget(view,0,0);

    setupContent();


}

void WSegments::setupContent()
{
    model = new MSegments;
    view->setModel(model);
}

void WSegments::refresh(){
    model->refresh();
}

