#include "udpsock.h"
#include <QDataStream>
#include <QDateTime>
#include <QFile>
#include "memory.h"

#include <iostream>
using namespace std;

UDPSock::UDPSock(QObject *parent) : QObject(parent)
{
    p_udpSocket = new QUdpSocket(this);
    p_udpSocketSrv = new QUdpSocket(this);
    PacketNum = 0;
    QObject::connect(p_udpSocketSrv, SIGNAL(readyRead()), SLOT(readMsg()));
    p_udpSocketSrv->bind(QHostAddress::AnyIPv4,Memory::get("rlsPort",30583).toInt());
    fname = "";
    position = 0;
}
UDPSock::~UDPSock(){
    p_udpSocket->deleteLater();
    p_udpSocketSrv->deleteLater();
}
void UDPSock::sendMsg(QString info,QString address, quint16 port){
    QByteArray Datagram;
    QDataStream out(&Datagram, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_4);
        out << info;
        p_udpSocket->writeDatagram(info.toLatin1(), QHostAddress(address), port);

}
void UDPSock::sendMsg(QByteArray info,QString address, quint16 port){
    QDateTime current = QDateTime::currentDateTime();
    uint tSafe = current.toTime_t();
    /*QByteArray Datagram;
    QDataStream out(&Datagram, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_6);
        out << info;*/
    for(auto dl = info.begin(); dl!=info.end();++dl){
        unsigned int bData = (unsigned char)*dl;
        cout << bData << " " << std::ends;
    }
    cout << "" << endl;
    p_udpSocket->writeDatagram(info, QHostAddress(address), port);
    fname = QString::number(PacketNum)+"."+QString::number(tSafe)+".src";
}
void UDPSock::SendCmdPacket(unsigned short BufferSize, unsigned char *Buffer, unsigned short CmdNum){
    ++PacketNum;
    unsigned char *TxBuffer = new unsigned char[10];
    unsigned char *DataPtrB = TxBuffer;
    *(unsigned int   *)(DataPtrB) = 0xABCDDCBA;
    DataPtrB += sizeof(unsigned int);
     *(unsigned short *)(DataPtrB) = CmdNum;
    DataPtrB += sizeof(unsigned short);
    *(unsigned int   *)(DataPtrB) = PacketNum;
    QByteArray ab;
    ab.resize(10+BufferSize);
    unsigned char *DataPtr = (unsigned char *)ab.data();
    memcpy(DataPtr,TxBuffer,10);
    DataPtr+=10;
    memcpy(DataPtr,Buffer,BufferSize);
    QString address = Memory::get("rlsIP","127.0.0.1").toString();
    cout << "send: "<< address.toStdString() << endl;
    sendMsg(ab,address,Memory::get("rlsPort",30583).toInt());
    packet.insert(PacketNum,CmdNum);
    position = 0;
    cnt = 0;
}
void UDPSock::readMsg(){
    //QFile fl(fname);
    //fl.open(QIODevice::Append);
    QByteArray Datagram;
        do {
            ++cnt;
            int size = p_udpSocketSrv->pendingDatagramSize();
            Datagram.resize(size);
            p_udpSocketSrv->readDatagram(Datagram.data(),size);
            //Memory::setData("srcData",Datagram.data(),size,position);
            //fl.write(Datagram);
            position+=size;
            cout << "dg: " << Datagram.size() << endl;
            int endIn = 0;
            for(auto dl = Datagram.begin(); dl!=Datagram.end();++dl){
                if(endIn < 30){
                    unsigned int bData = (unsigned char)*dl;
                    cout << bData << " " << std::ends;
                     ++endIn;
                }
            }
            cout << "" << endl;
        } while(p_udpSocketSrv->hasPendingDatagrams());
    //fl.close();
    cout << "count: " << cnt << endl;
}
