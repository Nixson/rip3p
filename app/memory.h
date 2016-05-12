#ifndef MEMORY_H
#define MEMORY_H

#include <QVariant>
#include <QSharedMemory>
#include "nomain/scobject.h"
#include "typelist.h"
#include "nomain/rdata.h"

class Memory
{
public:
    Memory();
    static QVariant get(QString name, QVariant def);
    static void set(QString,QVariant);
    static QMap<QString,QVariant> info;
    static QMap<QString,ScObject *> scL;
    static QMap<QString,Clowd> resultData;
    static QMap<QString,fdata_t*> dataLink;
    static Rdata *dLink;
    static void setData(QString name, void *from, int length, int start = 0);
    static void getData(QString name, void *to, int length, int start = 0);
};


#endif // MEMORY_H
