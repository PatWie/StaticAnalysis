#include "WImports.h"
#include <QGridLayout>
#include <QTreeView>
#include <QMap>

#include "../tia.h"

WImports::WImports(QWidget *parent) :
    QWidget(parent)
{
    QGridLayout *layout = new QGridLayout(this);
    view = new QTreeView ;
    view->setIndentation(0);
    layout->addWidget(view,0,0);

    setupContent();

}

void WImports::setupContent()
{
    model = new MImports;
    view->setModel(model);
}

void WImports::clear(){
    model->refresh();
}

void WImports::refresh(){
    model->refresh();
}

