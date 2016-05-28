#ifndef PMWORKER_H
#define PMWORKER_H

#include <QObject>
#include "typelist.h"
#include <mgl2/qt.h>



class PMworker : public QObject
{
    Q_OBJECT
public:
    explicit PMworker(QObject *parent = 0);

signals:
    void plot(mglGraph *data);

public slots:
    void syncSlot(QString type);
    void dataSlot(Clowd &dataA, Clowd &dataH);
    void plotSlot();
    void sharedSlot(int ship, QString type);
private:
    int m_count, AngleMin, AngleMax, OffsetMin, OffsetMax, Size, maxColor, leRasterPeriod, Lay;
    uint MaxBarier;
    Clowd m_data, histA, histY;
    mglGraph *colorMap;
};

#endif // PMWORKER_H
