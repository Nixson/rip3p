#include "worker.h"
#include "memory.h"


Worker::Worker(QObject *parent) : QObject(parent)
{
    OriginalPulseRe = new float[BLOCKLANGTH];
    OriginalPulseIm = new float[BLOCKLANGTH];
    memGarg.setKey("ripgorizontalar");
    memGph.setKey("ripgorizontalph");
    memVarg.setKey("ripverticalar");
    memVph.setKey("ripverticalph");
    isAttach = false;
    maxColor = 0.0;
    colorStep = 0.0;
}

void Worker::loadSrc(QByteArray &data){
    int step = BLOCKLANGTH*32; // определяем количество отсчетов
    int cntD = data.count();
    initPulse(Memory::get("leSubBufNum",0).toInt(),Memory::get("leFreq",0.0).toDouble());
    int bSize = cntD/step;
    int bNum[4] = {0, 16, 15, 31};
    //dataDouble = new double[cntD/8];
    dataFloat.resize(cntD/8);
    float *dataFloatAddr = dataFloat.data();

    for( int bStep = 0; bStep < bSize; bStep++){
        //номер пачки
        int packet = bStep*step;
        // берем 0, 16, 15, 31 блоки
        //0
        for( int pStep = 0; pStep < 4; pStep++){
            for( int sStep = 0; sStep < BLOCKLANGTH; sStep++){
                int vl = (unsigned int)data[packet+bNum[pStep]*BLOCKLANGTH+sStep];
                if(vl < 0) vl += MAXBYTE;
                *dataFloatAddr++ = (GLfloat)(vl - 128);
            }
        }
    }
    Size = bSize;
    Memory::set("Size",bSize);
    emit save();
    Buffer();
    attach();
    run();
    emit log("Обработка завершена");
}
void Worker::loadFinished(QByteArray &data){
    int binarySize = data.size()/8;
    Size = binarySize/ArgMax;
    Memory::set("Size",Size);
    emit save();
    if(histGA.size()!=Size*BLOCKLANGTH){
        histGA.clear();
        histVA.clear();
        histGY.clear();
        histVY.clear();
        histGA.resize(Size*BLOCKLANGTH);
        histVA.resize(Size*BLOCKLANGTH);
        histGY.resize(Size*BLOCKLANGTH);
        histVY.resize(Size*BLOCKLANGTH);
    }
    Clowd bsA;
    Clowd bsP;
    bsA.resize(binarySize);
    bsP.resize(binarySize);
    char *dataFile = data.data();
    memcpy(bsA.data(),dataFile,data.size()/2);
    dataFile += binarySize*4;
    memcpy(bsP.data(),dataFile,data.size()/2);
    maxColor = 0.0;
    float paxPh = 0.0;

    for( int step = 0; step < Size; ++step){
        for(int mk = 0; mk < BLOCKLANGTH; ++mk){
            int position = mk+step*BLOCKLANGTH;
            if(mk >= ArgMax){
                histGA[position] = 0;
                histVA[position] = 0;
                histGY[position] = 0;
                histVY[position] = 0;
            }
            else {
                int positionR = mk+step*ArgMax;
                histGA[position] = bsA[positionR];
                histVA[position] = bsA[positionR];
                if(maxColor < bsA[positionR])
                    maxColor = bsA[positionR];
                histGY[position] = bsP[positionR]*180/M_PI;
                histVY[position] = bsP[positionR]*180/M_PI;
                if(paxPh < bsP[positionR])
                    paxPh = bsP[positionR];
            }
        }
    }
    bsA.clear();
    bsP.clear();
    float norm = (float)QUINT16_SIZE/maxColor;

    for( int step = 0; step < Size; ++step){
        for(int mk = 0; mk < BLOCKLANGTH; ++mk){
            int position = mk+step*BLOCKLANGTH;
            float ps = round(histGA[position]*norm);
            histGA[position] = ps;
            histVA[position] = ps;
        }
    }
    emit resultGorizontal(histGA,histGY);
    emit resultVertical(histVA,histVY);
}
void Worker::sync(){
    ArgMin = Memory::get("ArgMin",0).toInt();
    ArgMax = Memory::get("ArgMax",1024).toInt();
}
void Worker::attach(){
    int blockSize = Size*BLOCKLANGTH*sizeof(float);
    if(memGarg.attach()){
        isAttach = true;
    }
    int sizeMem = memGarg.size();
    if(sizeMem > 0 && blockSize > sizeMem){
        memGarg.detach();
        memGarg.create(blockSize);
        memVarg.detach();
        memVarg.create(blockSize);
        memGph.detach();
        memGph.create(blockSize);
        memVph.detach();
        memVph.create(blockSize);
    }else {
        memVarg.attach();
        memGph.attach();
        memVph.attach();
    }
    if(histGA.size()!=Size*BLOCKLANGTH){
        histGA.clear();
        histGA.resize(Size*BLOCKLANGTH);
        histVA.clear();
        histVA.resize(Size*BLOCKLANGTH);
        histGY.clear();
        histGY.resize(Size*BLOCKLANGTH);
        histVY.clear();
        histVY.resize(Size*BLOCKLANGTH);
    }
}
void Worker::compile(int startPos,int position,int iNum){
    //float *tmpGA,*tmpVA;
    int numPos = iNum*BLOCKLANGTH;
    //int sInum = iNum;
    for(int bStep = startPos+1; bStep < position; bStep+=2 ){
        histGY[numPos] = (float)round(ResUlst[bStep]);
        histVY[numPos] = (float)round(ResUlstY[bStep]);
        ++numPos;
    }
    float maxColorArg = maxColor*0.7;
    float norm = (float)QUINT16_SIZE/maxColorArg;
/*    float *dataResultIntG = histGA.data();
    float *dataResultIntV = histVA.data();*/
    int start = startPos;
    numPos = iNum*BLOCKLANGTH;
    if(maxColor != colorStep){
        numPos = 0;
        start = 0;
        //sInum = 0;
        colorStep = maxColor;
    }
    for(int bStep = start; bStep < position; bStep+=2){
        float rsp = (float)round(ResUlst[bStep]*norm);
        if(rsp >= (float)QUINT16_SIZE)
            rsp = (float)QUINT16_SIZE-1.0;
        histGA[numPos] = rsp;
        rsp = (float)round(ResUlstY[bStep]*norm);
        if(rsp >= (float)QUINT16_SIZE)
            rsp = (float)QUINT16_SIZE-1.0;
        histVA[numPos] = rsp;
        ++numPos;
    }

    int progressValue = 100*iNum/Size;
    if(progressValue > lastProgress){
        lastProgress = progressValue;
        emit progress(lastProgress);
            int range = numPos*sizeof(float);
            Memory::setData("vGorizontalAr",histGA.data(),range);
            /*memGarg.lock();
            memcpy(memGarg.data(),histGA.data(),range);
            memGarg.unlock();*/
            Memory::setData("vGorizontalPh",histGY.data(),range);
            /*
            memGph.lock();
            memcpy(memGph.data(),histGY.data(),range);
            memGph.unlock();*/

            Memory::setData("vVerticalAr",histVA.data(),range);
            /*memVarg.lock();
            memcpy(memVarg.data(),histVA.data(),range);
            memVarg.unlock();*/
            Memory::setData("vVerticalPh",histVY.data(),range);
            /*
            memVph.lock();
            memcpy(memVph.data(),histVY.data(),range);
            memVph.unlock();
            */
            emit shared(iNum);
    }
}
void Worker::run(){
    int sizeResp = Size*BLOCKLANGTH*2;
    ResUlst = Memory::resultData["Gorizontal"];
    ResUlstY = Memory::resultData["Vertical"];
    if(ResUlst.size() != sizeResp){
        ResUlst.clear();
        ResUlst.resize(sizeResp);
    }
    if(ResUlstY.size() != sizeResp){
        ResUlstY.clear();
        ResUlstY.resize(sizeResp);
    }
    int step = 0;
    int lastStep = 0;
    lastProgress = 0;
    for(int iNum = 0; iNum < Size; iNum++){
        float *DataBuf;
        DataBuf = dataFloat.data()+iNum*BLOCKRANGE;
        Math1(BLOCKRANGE,DataBuf);
        float index = 0;
        for(int i=0; i<BLOCKLANGTH; i++)
        {
               ResUlst[step] = ResXXAbs[i];
               ResUlstY[step] = ResYYAbs[i];
               if (maxColor < ResXXAbs[i])
                   maxColor = ResXXAbs[i];
               if (maxColor < ResYYAbs[i])
                   maxColor = ResYYAbs[i];
               ++step;
               // !!! значения аргумента в диапазоне от -180 до 180
               index = round(ResXXAng[i]+180); // округление аргумента для определения индекса ячейки
               // приведение фазы к диапазону от 0 до 360
               while(index >= 360) index -= 360;
               while(index < 0) index += 360;
               // накопление значений
               ResUlst[step] = index;
               // !!! значения аргумента в диапазоне от -180 до 180
               index = round(ResYYAng[i]+180); // округление аргумента для определения индекса ячейки
               // приведение фазы к диапазону от 0 до 360
               while(index >= 360) index -= 360;
               while(index < 0) index += 360;
               // накопление значений
               ResUlstY[step] = index;
               ++step;
        }
        compile(lastStep,step,iNum);
        lastStep = step;
    }
    emit progress(100);
    Memory::resultData["Gorizontal"] = ResUlst;
    Memory::resultData["Vertical"] = ResUlstY;
    emit resultGorizontal(histGA,histGY);
    emit resultVertical(histVA,histVY);
    clear();
}
void Worker::initPulse(int leSubBufNum, double leFreq){
    unsigned int BufSize = leSubBufNum*BLOCKLANGTH;
    memset(OriginalPulseRe,0,BLOCKLANGTH*sizeof(float));
    memset(OriginalPulseIm,0,BLOCKLANGTH*sizeof(float));
    for(unsigned int i=1; i < BufSize;i++)
    {
           if(i < 10)
           {
               float re = 128*cos(2*M_PI*i/Fd*leFreq*1e6);
               OriginalPulseRe[i] =re;
               float im = 128*sin(2*M_PI*i/Fd*leFreq*1e6);
               OriginalPulseIm[i] = im;
           }
    }
}
void Worker::clear(){
        delete[] ResXXRe;
        delete[] ResXXIm;
        delete[] ResYYRe;
        delete[] ResYYIm;

        delete[] ResXXAbs;
        delete[] ResXXAng;
        delete[] ResYYAbs;
        delete[] ResYYAng;

        delete[] ResXXPhase;
        delete[] ResYYPhase;


        delete[] a0XXsv0;
        delete[] a0XXsv1;
        delete[] a1XXsv0;
        delete[] a1XXsv1;
        delete[] a0YYsv0;
        delete[] a0YYsv1;
        delete[] a1YYsv0;
        delete[] a1YYsv1;
}
void Worker::Buffer(){
    ResXXRe = new float[BLOCKLANGTH];
    ResXXIm = new float[BLOCKLANGTH];
    ResYYRe = new float[BLOCKLANGTH];
    ResYYIm = new float[BLOCKLANGTH];

    ResXXAbs = new float[BLOCKLANGTH];
    ResXXAng = new float[BLOCKLANGTH];
    ResYYAbs = new float[BLOCKLANGTH];
    ResYYAng = new float[BLOCKLANGTH];

    ResXXPhase = new float[BLOCKLANGTH];
    ResYYPhase = new float[BLOCKLANGTH];

    a0XXsv0 = new float[BLOCKLANGTH];
    a0XXsv1 = new float[BLOCKLANGTH];
    a1XXsv0 = new float[BLOCKLANGTH];
    a1XXsv1 = new float[BLOCKLANGTH];
    a0YYsv0 = new float[BLOCKLANGTH];
    a0YYsv1 = new float[BLOCKLANGTH];
    a1YYsv0 = new float[BLOCKLANGTH];
    a1YYsv1 = new float[BLOCKLANGTH];
}
void Worker::MyCorrelation(float* in, int dataSize, float* kernel, int kernelSize, float* out)
{
        int i, j, k;

        // check validity of params
        if(!in || !out || !kernel) return;
        if(dataSize <=0 || kernelSize <= 0) return;

        // start convolution from out[kernelSize-1] to out[dataSize-1] (last)
        for(i = kernelSize-1; i < dataSize; ++i)
        {
                out[i] = 0;                             // init to 0 before accumulate

                for(j = i, k = 0; k < kernelSize; --j, ++k)
                        out[i] += in[j] * kernel[k];
        }

        // convolution from out[0] to out[kernelSize-2]
        for(i = 0; i < kernelSize - 1; ++i)
        {
                out[i] = 0;                             // init to 0 before sum

                for(j = i, k = 0; j >= 0; --j, ++k)
                        out[i] += in[j] * kernel[k];
        }

        return;
}
void Worker::Math1(unsigned int BufSize, float *DataBuf)
{
     float *a0XX, *a1XX, *a0YY, *a1YY;

     unsigned int Size = BufSize/4;

     a0XX = DataBuf+Size*0;
     a1XX = DataBuf+Size*1;
     a0YY = DataBuf+Size*2;
     a1YY = DataBuf+Size*3;


     MyCorrelation(a0XX, Size, OriginalPulseRe, Size, a0XXsv0);
     MyCorrelation(a0XX, Size, OriginalPulseIm, Size, a0XXsv1);
     MyCorrelation(a1XX, Size, OriginalPulseRe, Size, a1XXsv0);
     MyCorrelation(a1XX, Size, OriginalPulseIm, Size, a1XXsv1);
     MyCorrelation(a0YY, Size, OriginalPulseRe, Size, a0YYsv0);
     MyCorrelation(a0YY, Size, OriginalPulseIm, Size, a0YYsv1);
     MyCorrelation(a1YY, Size, OriginalPulseRe, Size, a1YYsv0);
     MyCorrelation(a1YY, Size, OriginalPulseIm, Size, a1YYsv1);

     for(unsigned int i=0; i<Size; i++)
     {
            // произведение сигналов с двух антенн в режиме ХХ с комплексным сопряжением
                        // Изменил знаки, как в YY, стало лучше, но сдвиг углов между гор. и верт. поляризац. около 3 град.
            ResXXRe[i] = (a0XXsv0[i]*a1XXsv0[i] + a0XXsv1[i]*a1XXsv1[i])/1152/8;//*cos(M_PI/2);

                        // 	ResXXRe[i] = -(a0XXsv0[i]*a1XXsv0[i] + a0XXsv1[i]*a1XXsv1[i])/1152/8;//*cos(M_PI/2);
            // изменён порядок вычисления нумерации антенн 2015.01.31
            ResXXIm[i] = (a0XXsv0[i]*a1XXsv1[i] - a0XXsv1[i]*a1XXsv0[i])/1152/8;//*sin(M_PI/2);
                       // - заменил на +
                       // ResXXIm[i] = (-a0XXsv0[i]*a1XXsv1[i] - a0XXsv1[i]*a1XXsv0[i])/1152/8;//*sin(M_PI/2);
            // вычисление фазы сигнала с антенны 0 в режиме ХХ
            if((a0XXsv1[i] != 0) && (a0XXsv0[i] != 0))

                 ResXXPhase[i] = 180*(atan2(a0XXsv1[i], a0XXsv0[i])+M_PI)/M_PI;
                                 //Убрал из   ResXXPhase[i]    +M_PI для устранения фазового сдвига между каналами - не помогло
                                // ResXXPhase[i] = 180*(atan2(a0XXsv1[i], a0XXsv0[i]))/M_PI;

            if((a0YYsv1[i] != 0) && (a0YYsv0[i] != 0))
                 ResYYPhase[i] = 180*(atan2(a0YYsv1[i], a0YYsv0[i])+M_PI)/M_PI;

            // вычисление модуля и аргумента произведения сигналов в режиме ХХ
            ResXXAbs[i] = pow(ResXXRe[i]*ResXXRe[i] + ResXXIm[i]*ResXXIm[i], 0.5);
            if(ResXXAbs[i] >=1e-13) ResXXAng[i] = RAD*(atan2(ResXXIm[i], ResXXRe[i]));
            else ResXXAng[i] = 0; // аргумент от -180 до 180 градусов

            // произведение сигналов с двух антенн в режиме YY с комплексным сопряжением
            // изменён порядок вычисления нумерации антенн 2015.01.31
            // !!!!! УБРАНА ИНВЕРСИЯ ПРОИЗВЕДЕНИЯ СИГНАЛОВ АНТЕНН ДЛЯ УСТРАНЕНИЯ СИСТЕМАТИЧЕСКОЙ
            // ПОГРЕШНОСТИ РАЗНОСТИ ФАЗ В 180 ГРАДУСОВ
            ResYYRe[i] = (a0YYsv0[i]*a1YYsv0[i] + a0YYsv1[i]*a1YYsv1[i])/1152/8;
            ResYYIm[i] = (a0YYsv0[i]*a1YYsv1[i] - a0YYsv1[i]*a1YYsv0[i])/1152/8;

            // вычисление модуля и аргумента произведения сигналов в режиме YY
            ResYYAbs[i] = pow(ResYYRe[i]*ResYYRe[i] + ResYYIm[i]*ResYYIm[i], 0.5);
            if(ResYYAbs[i] >= 1e-13) ResYYAng[i] = RAD*(atan2(ResYYIm[i], ResYYRe[i]));
            else ResYYAng[i] = 0;

     }
}
