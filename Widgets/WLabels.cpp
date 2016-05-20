#include "WLabels.h"
#include <QMessageBox>
#include <QInputDialog>

WLabels::WLabels(QWidget *parent) :
    QWidget(parent)
{
    model = new MLabels;
}


void WLabels::setLabelByDlg(address_t addr){


    QString oldLabel = exists(addr) ? get(addr) : "";
    QString newLabel = "";

    bool ok;
    do{
        if(newLabel != "")
            QMessageBox::critical(NULL,"Input error","label to long");
        newLabel = QInputDialog::getText(this, tr("Label"),tr("label name:"), QLineEdit::Normal,oldLabel, &ok);
    }while(newLabel.length()>16);

    if (ok && !newLabel.isEmpty())
        set(addr,newLabel);
    if (ok && newLabel.isEmpty())
        remove(addr);

}

void WLabels::setVariableByDlg(address_t addr){

    // dialog for create labels
    if(addr==NULL)
        return;

    QString oldLabel = exists(addr) ? get(addr) : "";
    QString newLabel = "";

    bool ok;
    do{
        if(newLabel != "")
            QMessageBox::critical(NULL,"Input error","varname to long");
        newLabel = QInputDialog::getText(this, tr("Variable"),tr("variable name:"), QLineEdit::Normal,oldLabel, &ok);
    }while(newLabel.length()>16);

    if (ok && !newLabel.isEmpty())
        set(addr,newLabel);
    if (ok && newLabel.isEmpty())
        remove(addr);

}

void WLabels::remove(address_t address)
{
    model->m_Labels.remove(address);
    emit updateLabel(address);
}

void WLabels::clear()
{
    model->m_Labels.clear();
}

void WLabels::set(address_t address, QString text)
{
    model->m_Labels.insert(address,text);
    emit updateLabel(address);

}

const bool WLabels::exists(address_t address) const{
    return model->m_Labels.contains(address);
}

QString WLabels::get(address_t address) const
{
    return model->m_Labels[address];
}
