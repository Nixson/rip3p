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
    PacketNum = 1;
    QObject::connect(p_udpSocket, SIGNAL(readyRead()), SLOT(readMsg()));
    fname = "";
}
UDPSock::~UDPSock(){
    p_udpSocket->deleteLater();
}
void UDPSock::sendMsg(QString info,QString address, quint16 port){
    QByteArray Datagram;
    QDataStream out(&Datagram, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_6);
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
    cout << info.toStdString() << endl;
        p_udpSocket->writeDatagram(info, QHostAddress(address), port);
        fname = QString::number(PacketNum)+"."+QString::number(tSafe)+".src";

}
void UDPSock::SendCmdPacket(unsigned short BufferSize, unsigned char *Buffer, unsigned short CmdNum){
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
    sendMsg(ab,Memory::get("rlsIP","127.0.0.1").toString(),Memory::get("rlsPort",30583).toInt());
    ++PacketNum;
}
void UDPSock::readMsg(){
    QFile fl(fname);
    fl.open(QIODevice::Append);
    QByteArray Datagram;
        do {
            Datagram.resize(p_udpSocket->pendingDatagramSize());
            p_udpSocket->readDatagram(Datagram.data(), Datagram.size());
            fl.write(Datagram);
            cout << "dg: " << Datagram.size() << endl;
            for(auto dl = Datagram.begin(); dl!=Datagram.end();++dl){
                unsigned int bData = (unsigned char)*dl;
                cout << bData << " " << ends;
            }
            cout << "" << endl;
        } while(p_udpSocket->hasPendingDatagrams());
    fl.close();
}
