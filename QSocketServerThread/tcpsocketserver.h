#ifndef TCPSOCKETSERVER_H
#define TCPSOCKETSERVER_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>

#define PDU_READY           0
#define PDU_BUSY            1
#define PDU_FRAME_REQ       2
#define PDU_FRAME_DATA      3

typedef struct {
    int length;
    int payload_type;
    char data[0];
}PduHeader;

class TcpSocketServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit TcpSocketServer();
    ~TcpSocketServer();

protected:
    virtual void incomingConnection(qintptr socketDescriptor) Q_DECL_OVERRIDE;

signals:
    void startConnectListen();

public slots:
    void socketListen();
    void socketReadyRead();
    void socketDisconnected();

private:
    void sendReadyMsg(QTcpSocket *tcpSocket);
    bool socketWriteData(QTcpSocket *tcpSocket, const char *buffer, int length);
    void sendDataMsg(QTcpSocket *tcpSocket);
    void sendBusyMsg(QTcpSocket *tcpSocket);

private:
    QTcpSocket *m_pTcpSocket;
    QByteArray m_socketBuffer;
    int m_nCount;
};

#endif
