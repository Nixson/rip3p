#include "plotmath.h"
#include "ui_plotmath.h"
#include "memory.h"



class PlotMathDraw : public mglDraw
{
public:
        void setPlotMath(PlotMath *pmath);
        int Draw(mglGraph *gr);
private:
        PlotMath *pm;
};
//-----------------------------------------------------
void PlotMathDraw::setPlotMath(PlotMath *pmath)
{
    pm = pmath;
}
int PlotMathDraw::Draw(mglGraph *gr)
{
        return pm->sample(gr);
}


int PlotMath::sample(mglGraph *gr){
    if(Amp.size()==0){
        int i, n=2000;
        mglData x(n),y(n),z(n),c(n);
        for(i=0;i<n;i++)
        {
          mreal t=M_PI*(mgl_rnd()-0.5), f=2*M_PI*mgl_rnd();
          x.a[i] = 0.9*cos(t)*cos(f);
          y.a[i] = 0.9*cos(t)*sin(f);
          z.a[i] = 0.6*sin(t);
          c.a[i] = cos(2*t);
        }
        //gr->SubPlot(2,2,0);
        gr->Title("Dots sample");  gr->Rotate(50,60);
        gr->Box();
        gr->Dots(x,y,z,c);
        gr->Crust(x,y,z,"#");
        return 0;
    }
    int realSize = 0;
    maxColor = 0;
    for( int x = 0; x < Size; x++){
        int packet = x*BLOCKLANGTH;
        for(int z = ArgMin; z < ArgMax; z++){
            int y = (int)Phase[packet+z];
            if(y >= PhMin && y <= PhMax){
                uint color = (uint)Amp[packet+z];
                if(color >= (uint)Barier){
                    if(color > (uint)maxColor)
                        maxColor = (int)color;
                    ++realSize;
                }
            }
        }
    }
    if(realSize==0){
        int i, n=2000;
        mglData x(n),y(n),z(n),c(n);
        for(i=0;i<n;i++)
        {
          mreal t=M_PI*(mgl_rnd()-0.5), f=2*M_PI*mgl_rnd();
          x.a[i] = 0.9*cos(t)*cos(f);
          y.a[i] = 0.9*cos(t)*sin(f);
          z.a[i] = 0.6*sin(t);
          c.a[i] = cos(2*t);
        }
        //gr->SubPlot(2,2,0);
        gr->Title("Dots sample");  gr->Rotate(50,60);
        gr->Box();
        gr->Dots(x,y,z,c);
        gr->Crust(x,y,z,"#");
        return 0;
    }
    int size = 0;
    mglData xV(realSize), yV(realSize),zV(realSize),cV(realSize),aV(realSize);
    double centerX = (double)Size/2;
    double centerY =  PhMin + (double) ( PhMax - PhMin ) /2;
    double centerZ = ArgMin + (double) ( ArgMax - ArgMin ) /2;



    for( int x = 0; x < Size; x++){
        int packet = x*BLOCKLANGTH;
        for(int z = ArgMin; z < ArgMax; z++){
            int y = (int)Phase[packet+z];
            if(y >= PhMin && y <= PhMax){
                uint color = (uint)Amp[packet+z];
                if(color >= (uint)Barier){
                    double xS = ((double)x-centerX)/Size;
                    xV.a[size] = xS;
                    yV.a[size] = ((double)y - centerY)/(double)PhMax;
                    zV.a[size] = ((double)z - centerZ)/(double)ArgMax;
                    if(maxColor==0){
                        cV.a[size] = 0;
                        aV.a[size] = 1;
                    }else{
                        cV.a[size] = color/maxColor;
                        aV.a[size] = color/maxColor;
                    }
                    ++size;
                }
            }
        }
    }
    gr->Box();
    gr->Dots(xV,yV,zV,cV,aV);
    //gr->Crust(xV,yV,zV,"#");
    gr->Alpha(true);
    return 0;
}
PlotMath::PlotMath(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlotMath)
{
    ui->setupUi(this);
    PlotMathDraw *pmd = new PlotMathDraw();
    pmd->setPlotMath(this);
    QMGL = new QMathGL();
    QMGL->setDraw(pmd);
    QMGL->setZoom(true);
    QMGL->setRotate(true);
    QMGL->setLight(true);
    QMGL->setGrid(true);
    //QMGL->setAlpha(true);
    //QMGL->setViewYZ(true);
    QMGL->update();
    ui->mathWidget->addWidget(QMGL);
    QMGL->update();
    connect(QMGL,&QMathGL::customZoom,this,&PlotMath::reZoom);
//    connect(QMGL,&QMathGL::,this,&PlotMath::reZoom);
    worker = new PMworker();
    worker->moveToThread(&mathThread);
    connect(&mathThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this,&PlotMath::syncSignal,worker,&PMworker::syncSlot);
    connect(this,&PlotMath::dataSignal,worker,&PMworker::dataSlot);
    //connect(this,&PlotMath::sharedSignal,worker,&PMworker::sharedSlot);
    connect(this,&PlotMath::plotSignal,worker,&PMworker::plotSlot);
    connect(worker,&PMworker::plot,this,&PlotMath::result);
    mathThread.start();
    sharedTimer = new QTimer();
    sharedTimer->setInterval(40);
    connect(sharedTimer,&QTimer::timeout,this,&PlotMath::sharedNow);
    isShared = false;
}

PlotMath::~PlotMath()
{
    delete ui;
    mathThread.quit();
    mathThread.wait();
}
void PlotMath::resizeEvent(QResizeEvent *event){
    Q_UNUSED(event);
    QSize qs= QMGL->size();
    QMGL->setSize(qs.width(),qs.height()-20);
    QMGL->update();
}
void PlotMath::showEvent(QShowEvent *event){
    Q_UNUSED(event);
    QSize qs= QMGL->size();
    QMGL->setSize(qs.width(),qs.height());
    QMGL->update();
}
void PlotMath::reZoom(double x1, double y1, double x2, double y2, double tet, double phi, double per){
    Q_UNUSED(x1);
    Q_UNUSED(y1);
    Q_UNUSED(x2);
    Q_UNUSED(y2);
    Q_UNUSED(tet);
    Q_UNUSED(phi);
    Q_UNUSED(per);
}
void PlotMath::setType(QString type){
    Type = type;
}
void PlotMath::setBuf(Clowd &Ar, Clowd &Ph){
    if(sharedTimer->isActive()){
        sharedTimer->stop();
    }
    Amp = Ar;
    Phase = Ph;
    QMGL->refresh();
    //emit dataSignal(Ar, Ph);
}
void PlotMath::plot(){
    QMGL->refresh();
    //emit plotSignal();
}
void PlotMath::syncSlot(){
    ArgMin = Memory::get("ArgMin",0).toInt();
    ArgMax = Memory::get("ArgMax",1024).toInt();
    PhMin = 0;
    PhMax = Memory::get("PhMin",180).toInt();
    leRasterPeriod = Memory::get("leRasterPeriod"+Type,10).toInt();
    Lay = Memory::get("PlotPLay"+Type,0).toInt();
    Barier = Memory::get("Barier",0).toInt();
    maxColor = Memory::get("colorValue",0).toInt();
    int size = Memory::get("Size",400).toInt();
    if(size!=Size){
        Size = size;
    }
    /*emit syncSignal(Type);
    emit plotSignal();*/
//    plot();
}
void PlotMath::result(mglGraph *data){
    Q_UNUSED(data);
//    QMGL->setDraw(data);
    QMGL->update();
    QMGL->refresh();
}
void PlotMath::sharedNow(){
    if(isShared){
        isShared = false;
    }
    else return;
    emit sharedSignal(sharedShip,Type);
}
void PlotMath::shared(int shp){
    isShared = true;
    sharedShip = shp;
    if(!sharedTimer->isActive()){
        sharedTimer->start();
    }
}
