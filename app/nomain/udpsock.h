#ifndef UDPSOCK_H
#define UDPSOCK_H

#include <QObject>
#include <QUdpSocket>

class UDPSock : public QObject
{
    Q_OBJECT
public:
    explicit UDPSock(QObject *parent = 0);

signals:
    void sendSignal();

public slots:
    void readMsg();
    void sendMsg(QString info);
private:
    QUdpSocket* p_udpSocket;
};

#endif // UDPSOCK_H
