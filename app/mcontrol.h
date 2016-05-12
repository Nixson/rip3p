#ifndef MCONTROL_H
#define MCONTROL_H

#include "typelist.h"
#include <QObject>
#include <QSettings>
#include <QThread>
#include <QMdiArea>
#include <QMdiSubWindow>

#include "nomain/debugdialog.h"
#include "nomain/scobject.h"
#include "gr/gr3d.h"
#include "gr/plotpolarization.h"
#include "gr/plotraster.h"

#include "plugwin.h"
#include "worker.h"

class MControl : public QObject
{
    Q_OBJECT
    QThread workerThread;
public:
    explicit MControl(QObject *parent = 0);
    ~MControl();
    void setAction(QString name, QVariant value);
    void winOpen(QString winName);
    void showGr3D(QString sType);
    void showPlotPolarization(QString sType);
    void showPlotRastr(QString sType);
    void setDock(QMap<QString,QDockWidget *> &dock);
    void init();
    void load();
    DebugDialog* debug;
    PlugWin* plugin;
    QMdiSubWindow *gr3dVmdi, *gr3dGmdi, *grPlotVmdi, *grPlotGmdi, *grRPlotVmdi, *grRPlotGmdi;
    bool isgr3dVmdi, isgr3dGmdi, isgrPlotVmdi, isgrPlotGmdi, isgrRPlotVmdi, isgrRPlotGmdi;
    Gr3D *gr3dV, *gr3dG;
    PlotPolarization *grPlotV, *grPlotG;
    PlotRaster *grRPlotV, *grRPlotG;
    void setMDI(QMdiArea *lnk);
    QMdiArea *area;
    ScObject *scV, *scG;

signals:
    void loadSrc(QByteArray &data);
    void loadFinished(QByteArray &data);
    void setProgress(int);
    void setLog(QString);
    void sync();
    void loadPlugin(Plugs &plugin);
    void hidePlugin(Plugs &plugin);

public slots:
    void isgr3dVmdiHide();
    void isgr3dGmdiHide();
    void isgrPlotVmdiHide();
    void isgrPlotGmdiHide();
    void isgrRPlotVmdiHide();
    void isgrRPlotGmdiHide();
    void saveConfig();
    void log(QString);
    void progress(int);
    void shared(int);
    void resultXX(Clowd &dataA, Clowd &dataH);
    void resultYY(Clowd &dataA, Clowd &dataH);
    void loadData(QString fileName, QByteArray &data);

private:
    QSettings* settings;
    Worker *worker;
    bool hasGData, hasVData;
};

#endif // MCONTROL_H
