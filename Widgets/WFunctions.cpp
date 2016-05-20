#include "WFunctions.h"
#include <QGridLayout>
#include <QTreeView>
#include <QMap>

#include "../tia.h"

WFunctions::WFunctions(QWidget *parent) :
    QWidget(parent)
{
    QGridLayout *layout = new QGridLayout(this);
    view = new QTreeView ;
    view->setIndentation(0);
    layout->addWidget(view,0,0);

    setupContent();

    connect(view,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(onDoubleClicked(QModelIndex)));
}

void WFunctions::setupContent()
{
    model = new MFunctions;
    view->setModel(model);
}


void WFunctions::onDoubleClicked(QModelIndex idx)
{
    emit gotoAddress(model->functionStart(idx.row()));
}


void WFunctions::addApiCall(address_t addr, APIData_t dat)
{
    model->addApiCall(addr,dat);

}


const APIData_t WFunctions::getApiCall(address_t addr) const
{
    return model->getApiCall(addr);
}

QMap<address_t, APIData_t > *WFunctions::getApiCalls()
{
    return model->getApiCalls();
}

const bool WFunctions::isApiCall(address_t target) const
{
    return model->isApiCall(target);
}

void WFunctions::clear(){
    model->clear();
}

void WFunctions::refresh(){
    model->refresh();
}


void WFunctions::addFunction(address_t start_addr, address_t ret_address){
    if(ret_address == NULL)
        ret_address = start_addr;

    model->addFunction(start_addr,ret_address);
}
