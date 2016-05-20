#ifndef WFUNCTIONS_H
#define WFUNCTIONS_H

#include "../Models/MFunctions.h"
#include "../Global.h"
#include "../Models/MTarget.h"

#include <QWidget>
#include <QMap>
#include <QTreeView>
#include <QItemSelection>


class WFunctions : public QWidget
{
    Q_OBJECT


    QMap<address_t, address_t > m_Functions;   // <start,end>

public:
    WFunctions(QWidget *parent=0);

    QMap<address_t, APIData_t> *getApiCalls();
    const APIData_t getApiCall(address_t addr) const;
    const bool isApiCall(address_t target) const;
    void addApiCall(address_t addr, APIData_t dat);
    void addFunction(address_t start_addr, address_t ret_address=NULL);
    void clear();

public slots:
    void refresh();
signals:
    void selectionChanged (const QItemSelection &selected);
    void gotoAddress(address_t addr);

private slots:
    void onDoubleClicked(QModelIndex idx);
private:
    void setupContent();


    MFunctions *model;
    QTreeView *view;

};

#endif // WFUNCTIONS_H
