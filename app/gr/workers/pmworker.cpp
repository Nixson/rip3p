#include "pmworker.h"
#include "memory.h"

PMworker::PMworker(QObject *parent) : QObject(parent)
{

}
void PMworker::syncSlot(QString type){
    int nSize = Memory::get("Size",0).toInt();
    if(nSize != Size){
        Size = nSize;
        histA.clear();
        histA.resize(Size*BLOCKLANGTH);
        histY.clear();
        histY.resize(Size*BLOCKLANGTH);
    }
    AngleMin = 180-Memory::get("PhMin",0).toInt();
    AngleMax = Memory::get("PhMin",0).toInt()+180;
    OffsetMin = Memory::get("ArgMin",0).toInt();
    OffsetMax = Memory::get("ArgMax",1024).toInt();
    maxColor = Memory::get("colorValue",0).toInt();
    MaxBarier = Memory::get("Barier",100).toInt();
    leRasterPeriod = Memory::get("leRasterPeriod"+type,10).toInt();
    Lay = Memory::get("PlotPLay"+type,0).toInt();
}
void PMworker::sharedSlot(int ship, QString type){
    if(histA.size()==0){
        syncSlot(type);
    }
    QString nameA,nameP;
    if(type=="Vertical"){
        nameA = "vVerticalAr";
        nameP = "vVerticalPh";
    }
    else{
        nameA = "vGorizontalAr";
        nameP = "vGorizontalPh";
    }
    int shipPosition = ship*BLOCKLANGTH*sizeof(float);
    Memory::getData(nameA,histA.data(),shipPosition);
    Memory::getData(nameP,histY.data(),shipPosition);
    plotSlot();
}
void PMworker::dataSlot(Clowd &dataA, Clowd &dataH){
    histA = dataA;
    histY = dataH;
    plotSlot();
}
void PMworker::plotSlot(){
    if(histA.size()==0){
        return;
    }
    int realSize = 0;
    for( int x = 0; x < Size; x++){
        int packet = x*BLOCKLANGTH;
        for(int z = OffsetMin; z < OffsetMax; z++){
            int y = (int)histY[packet+z];
            if(y >= AngleMin && y <= AngleMax){
                uint color = (uint)histA[packet+z];
                if(color >= MaxBarier){
                    if(color > (uint)maxColor)
                        maxColor = color;
                    ++realSize;
                }
            }
        }
    }
    if(realSize==0){
        mglGraph *gr = new mglGraph();
        gr->Box();
        emit plot(gr);
        return;
    }
    int size = 0;
    mglData xV(realSize), yV(realSize),zV(realSize),cV(realSize),aV(realSize);


    for( int x = 0; x < Size; x++){
        int packet = x*BLOCKLANGTH;
        for(int z = OffsetMin; z < OffsetMax; z++){
            int y = (int)histY[packet+z];
            if(y >= AngleMin && y <= AngleMax){
                uint color = (uint)histA[packet+z];
                if(color >= MaxBarier){
                    xV.a[size] = x;
                    yV.a[size] = y;
                    zV.a[size] = z;
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
    mglGraph *gr = new mglGraph();
    gr->Box();
    gr->Dots(xV,yV,zV,cV,aV);
    gr->Crust(xV,yV,zV,"#");
    gr->Alpha(true);
    emit plot(gr);
}
