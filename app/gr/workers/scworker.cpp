#include "scworker.h"
#include "memory.h"

ScWorker::ScWorker(QObject *parent) : QObject(parent)
{
    syncSlot();
}
void ScWorker::syncSlot(){
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
    delY = (GLfloat)32.0f/(181-Memory::get("PhMin",0).toInt());

    centerX = (GLfloat)Size/2;
    centerY =  AngleMin + (GLfloat) ( AngleMax - AngleMin ) /2;
    centerZ = OffsetMin + (GLfloat) ( OffsetMax - OffsetMin ) /2;
}
void ScWorker::sharedSlot(int ship, QString type){
    if(histA.size()==0){
        syncSlot();
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
void ScWorker::dataSlot(Clowd &dataA, Clowd &dataH){
    histA = dataA;
    histY = dataH;
    plotSlot();
}
void ScWorker::plotSlot(){
    if(histA.size() == 0)
        return;
    centerX = (GLfloat)Size/2;
    centerY =  AngleMin + (GLfloat) ( AngleMax - AngleMin ) /2;
    centerZ = OffsetMin + (GLfloat) ( OffsetMax - OffsetMin ) /2;
    int realSize = 0;
    m_data.clear();
    //размер = длина * ширина * 24 точки по 6 координат в каждой
    m_data.resize( (OffsetMax-OffsetMin) * Size * 144);
    m_dataLink = m_data.data();

    for( int x = 0; x < Size; x++){
        //номер пачки
        int packet = x*BLOCKLANGTH;
        for(int z = OffsetMin; z < OffsetMax; z++){
            int y = (int)histY[packet+z];
            if(y >= AngleMin && y <= AngleMax){
                uint color = (uint)histA[packet+z];
                if(color >= MaxBarier){
                    plotFlower((int)x,(int)y,(int)z,color);
                    realSize++;
                }
            }
        }

    }
    m_data.resize(realSize*24*6);
    emit plot(m_data);
}
void ScWorker::plotFlower(int x,int y,int z, uint color){
    double NormalColor = (double)(QUINT16_SIZE);
    int nColor = (int)MAXBYTE*((double)color)/NormalColor;
    if(nColor > MAXBYTE)
        nColor = MAXBYTE;
    //appendPoint(x,y,z,nColor);
    appendPointObj(x-1,y,z-1,nColor);
    appendPointObj(x+1,y,z-1,nColor);
    appendPointObj(x,y+1,z,nColor);
    appendPointObj(x-1,y,z+1,nColor);
    appendPointObj(x+1,y,z+1,nColor);
    appendPointObj(x,y+1,z,nColor);
    appendPointObj(x+1,y,z-1,nColor);
    appendPointObj(x+1,y,z+1,nColor);
    appendPointObj(x,y+1,z,nColor);
    appendPointObj(x-1,y,z-1,nColor);
    appendPointObj(x-1,y,z+1,nColor);
    appendPointObj(x,y+1,z,nColor);

    appendPointObj(x-1,y,z-1,nColor);
    appendPointObj(x+1,y,z-1,nColor);
    appendPointObj(x,y-1,z,nColor);
    appendPointObj(x-1,y,z+1,nColor);
    appendPointObj(x+1,y,z+1,nColor);
    appendPointObj(x,y-1,z,nColor);
    appendPointObj(x+1,y,z-1,nColor);
    appendPointObj(x+1,y,z+1,nColor);
    appendPointObj(x,y-1,z,nColor);
    appendPointObj(x-1,y,z-1,nColor);
    appendPointObj(x-1,y,z+1,nColor);
    appendPointObj(x,y-1,z,nColor);
}
void ScWorker::appendPointObj(int x,int y,int z, int color){
    *(m_dataLink++) = (centerX-x)/MAXBYTEFLOAT;
    *(m_dataLink++) = (centerY-y)/(MAXBYTEFLOAT*delY);
    *(m_dataLink++) = -(centerZ-z)/MAXBYTEFLOAT;
    if(color == 0) {
        *(m_dataLink++) = 0;
        *(m_dataLink++) = 0;
        *(m_dataLink++) = 0;
    }
    else {
        *(m_dataLink++) = (float)color/MAXBYTEFLOAT;
        *(m_dataLink++) = 0.0f;
        *(m_dataLink++) = (float)(MAXBYTEFLOAT - color)/MAXBYTEFLOAT;
    }
}
