#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QSharedMemory>
#include "typelist.h"

class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(QObject *parent = 0);

signals:
    void save();
    void log(QString);
    void progress(int);
    void shared(int);
    void resultGorizontal(Clowd &dataArg,Clowd &dataPh);
    void resultVertical(Clowd &dataArg,Clowd &dataPh);

public slots:
    void loadSrc(QByteArray &data);
    void loadFinished(QByteArray &data);
    void sync();
private:
    QByteArray data;
    MathVector dataDouble;
    Clowd dataFloat;

    float *OriginalPulseRe, *OriginalPulseIm;
    void initPulse(int leSubBufNum, double leFreq);
    void Buffer();
    void clear();
    void run();
    void Math1(unsigned int BufSize, float *DataBuf);
    void MyCorrelation(float *in, int dataSize, float *kernel, int kernelSize, float *out);
    void compile(int startPos, int position, int iNum);
    void attach();
    bool isAttach;


    float *a0XXsv0;
    float *a0XXsv1;
    float *a1XXsv0;
    float *a1XXsv1;
    float *a0YYsv0;
    float *a0YYsv1;
    float *a1YYsv0;
    float *a1YYsv1;

    float *ResXXRe, *ResXXIm;
    float *ResYYRe, *ResYYIm;
    float *ResXXAbs, *ResXXAng;
    float *ResYYAbs, *ResYYAng;
    float *ResXXPhase, *ResYYPhase;
    float *ResXXReAvg, *ResXXImAvg;
    float *ResYYReAvg, *ResYYImAvg;
    QSharedMemory memGarg, memGph, memVarg,  memVph;
    Clowd histGA,histGY,histVA,histVY;
    int lastProgress;

    float maxColor, colorStep;

    int Size;

    Clowd ResUlst, ResUlstY;

    int ArgMin, ArgMax;

};

#endif // WORKER_H
