#ifndef MLABELS_H
#define MLABELS_H
#include "../Global.h"
#include <QObject>

class MLabels : public QObject
{
    Q_OBJECT
public:
    explicit MLabels(QObject *parent = 0);
    QMap<address_t,QString> m_Labels;        // <addr,text>
signals:

public slots:

};

#endif // MLABELS_H
