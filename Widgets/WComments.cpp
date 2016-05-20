#include "WComments.h"

#include <QMessageBox>
#include <QInputDialog>

WComments::WComments(QWidget *parent) :
    QWidget(parent), model(new MComments)
{
}

const bool WComments::exists(address_t addr) const
{
    return model->m_Comments.contains(addr);
}

QString WComments::get(address_t addr)
{
    return model->m_Comments[addr];
}


void WComments::set(address_t addr)
{
    QString oldComment = exists(addr) ? get(addr) : "";
    QString newComment = "";

    bool ok;

    do{
        if(newComment != "")
            QMessageBox::critical(NULL,"Input error","comment to long");

        newComment = QInputDialog::getText(this, tr("Comment"),tr("comment content:"), QLineEdit::Normal,oldComment, &ok);
    }while(newComment.length()>40);

    if (ok && !newComment.isEmpty())
        set(addr,newComment);
    if (ok && newComment.isEmpty())
        remove(addr);
}


void WComments::set(address_t addr, QString text)
{
    model->m_Comments.insert(addr,text);
    emit updateComment(addr);
}

void WComments::remove(address_t addr)
{
    model->m_Comments.remove(addr);
    emit updateComment(addr);
}

void WComments::clear()
{
    model->m_Comments.clear();
}
