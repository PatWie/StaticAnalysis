#ifndef MCOMMENTS_H
#define MCOMMENTS_H

#include <QObject>
#include <QMap>

#include "../Global.h"

class MComments : public QObject
{
    Q_OBJECT
public:
    explicit MComments(QObject *parent = 0);
    QMap<address_t, QString> m_Comments;

signals:

public slots:

};

#endif // MCOMMENTS_H
