#ifndef WLABELS_H
#define WLABELS_H

#include <QWidget>
#include <QMap>
#include "../Models/MLabels.h"
#include "../Global.h"

class WLabels : public QWidget
{
    Q_OBJECT
    MLabels *model;
public:
    explicit WLabels(QWidget *parent = 0);

signals:
    void updateLabel(address_t addr);

public slots:

    const bool exists(address_t address) const;
    void set(address_t address, QString text);
    QString get(address_t address) const;
    void remove(address_t address);
    void clear();

    void setVariableByDlg(address_t addr);
    void setLabelByDlg(address_t address);
};

#endif // WLABEL_H
