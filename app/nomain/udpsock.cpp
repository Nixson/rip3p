#include "udpsock.h"
#include <QDataStream>


UDPSock::UDPSock(QObject *parent) : QObject(parent), p_udpSocket(new QUdpSocket())
{
    QObject::connect(p_udpSocket, SIGNAL(readyRead()), SLOT(readMsg()));
}

void UDPSock::sendMsg(QString info){
    QByteArray Datagram;
    QDataStream out(&Datagram, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_6);
        out << info;
        p_udpSocket->writeDatagram(Datagram, QHostAddress::LocalHost, 2424);

}

void UDPSock::readMsg(){
    QByteArray Datagram;
        do {
            Datagram.resize(p_udpSocket->pendingDatagramSize());
            p_udpSocket->readDatagram(Datagram.data(), Datagram.size());
        } while(p_udpSocket->hasPendingDatagrams());

        QByteArray info;
        QDataStream in(&Datagram, QIODevice::ReadOnly);
        in >> info;
}
