#ifndef TCPSERVERDEMO_H
#define TCPSERVERDEMO_H

#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QTcpServer>
#include <QTcpSocket>

#define CONNECT_PORT    8888

#define PDU_READY           0
#define PDU_BUSY            1
#define PDU_FRAME_REQ       2
#define PDU_FRAME_DATA      3

typedef struct {
    int length;
    int payload_type;
    char data[0];
}PduHeader;

class TcpServerDemo : public QWidget
{
    Q_OBJECT

public:
    TcpServerDemo(QWidget *parent = 0);
    ~TcpServerDemo();

private:
    void initClientLayout();
    void initClientConnect();
    void sendReadyMsg(QTcpSocket *tcpSocket);
    bool socketWriteData(QTcpSocket *tcpSocket, const char *buffer, int length);
    void sendDataMsg(QTcpSocket *tcpSocket);

    QString getCurrentTime();

private slots:
    void btnSendClicked();
    void btnCloseClicked();

    void acceptSocketConnect();

    void socketDisconnected();
    void socketReadyRead();

private:
    QTextEdit*     m_pTextRead;
    QTextEdit*     m_pTextWrite;

    QPushButton*   m_pButtonSend;
    QPushButton*   m_pButtonClose;

    QTcpServer*    m_pTcpServer; //监听套接字
    QTcpSocket*    m_pTcpSocket; //通信套接字

    QByteArray m_socket_buffer;
};

#endif
