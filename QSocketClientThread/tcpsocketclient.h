#ifndef TCPSOCKETCLIENT_H
#define TCPSOCKETCLIENT_H

#include <QObject>
#include <QTcpSocket>

#define PDU_READY           0
#define PDU_BUSY            1
#define PDU_FRAME_REQ       2
#define PDU_FRAME_DATA      3

typedef struct {
    int length;
    int payload_type;
    char data[0];
}PduHeader;

class TcpSocketClient : public QObject
{
    Q_OBJECT
public:
    explicit TcpSocketClient(QObject *parent = 0);
    ~TcpSocketClient();

signals:

public slots:
    void socketConnected();
    void socketDisconnected();
    void socketReadyRead();

    void startConnectServer();

private:
    void sendReqMsg();
    bool socketWriteData(const char *buffer, int length);
    void initSocketConnect();

private:
    QTcpSocket *m_pTcpSocket;

    QByteArray m_socketBuffer;
};

#endif
