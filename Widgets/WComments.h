#ifndef WCOMMENTS_H
#define WCOMMENTS_H

#include <QWidget>

#include "../Models/MComments.h"
#include "../Global.h"

class WComments : public QWidget
{
    Q_OBJECT
    MComments *model;

public:
    explicit WComments(QWidget *parent = 0);

signals:
    void updateComment(address_t addr);

public slots:
    const bool exists(address_t addr) const;
    QString get(address_t addr);
    void set(address_t addr, QString text);
    void set(address_t addr);
    void remove(address_t addr);
    void clear();

};

#endif // WCOMMENTS_H
