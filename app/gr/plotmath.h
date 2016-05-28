#ifndef PLOTMATH_H
#define PLOTMATH_H

#include <QWidget>
#include <QTimer>
#include <QThread>
#include "typelist.h"
#include "workers/pmworker.h"
#include <mgl2/qmathgl.h>
#include <mgl2/qt.h>


namespace Ui {
class PlotMath;
}

class PlotMath : public QWidget
{
    Q_OBJECT
    QThread mathThread;

public:
    explicit PlotMath(QWidget *parent = 0);
    ~PlotMath();
    void setType(QString type);
    void setBuf(Clowd &Ar, Clowd &Ph);
    void shared(int);
    int sample(mglGraph *gr);
signals:
    void sync();
    void dataSignal(Clowd &Ar, Clowd &Ph);
    void syncSignal(QString type);
    void plotSignal();
    void sharedSignal(int ship, QString type);

public slots:
    void syncSlot();
    void plot();
    void result(mglGraph *data);
    void sharedNow();
    void reZoom(double x1, double y1, double x2, double y2, double tet, double phi, double per);

protected:
    void resizeEvent(QResizeEvent * event) Q_DECL_OVERRIDE;
    void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
private:
    Ui::PlotMath *ui;
    QMathGL *QMGL;
    int Size, Barier, ArgMin, ArgMax, PhMin, PhMax, sharedShip;
    int leRasterPeriod, Lay,maxColor;
    QString Type;
    Clowd Amp, Phase;
    PMworker *worker;
    QTimer *sharedTimer;
    bool isShared;
};

#endif // PLOTMATH_H
