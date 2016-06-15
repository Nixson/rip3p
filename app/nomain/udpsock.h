#ifndef UDPSOCK_H
#define UDPSOCK_H

#include <QObject>
#include <QUdpSocket>

class UDPSock : public QObject
{
    Q_OBJECT
public:
    explicit UDPSock(QObject *parent = 0);
    ~UDPSock();

signals:
    void sendSignal();

public slots:
    void readMsg();
    void sendMsg(QString info, QString address, quint16 port);
    void sendMsg(QByteArray info, QString address, quint16 port);
    void SendCmdPacket(unsigned short BufferSize, unsigned char *Buffer, unsigned short CmdNum);
private:
    QUdpSocket *p_udpSocket, *p_udpSocketSrv;
    int PacketNum;
    QMap <int, int> packet;
    QString fname;
    int position;
    int cnt;
};

#endif // UDPSOCK_H
