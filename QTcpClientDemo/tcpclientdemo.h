#ifndef TCPCLIENTDEMO_H
#define TCPCLIENTDEMO_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
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


class TcpClientDemo : public QWidget
{
    Q_OBJECT

public:
    TcpClientDemo(QWidget *parent = 0);
    ~TcpClientDemo();

private slots:
    void btnConnectClicked();
    void btnSendClicked();
    void btnCloseClicked();

    void socketConnected();
    void socketDisconnected();
    void socketReadyRead();

private:
    void initClientLayout();
    void initClientConnect();
    void send_req_msg();
    bool socketWriteData(const char *buffer, int length);
    void sendDataMsg(QString info);

    QString getCurrentTime();

private:
    QLabel*       m_pLabelIp;
    QLabel*       m_pLabelPort;

    QLineEdit*    m_pLineIp;
    QLineEdit*    m_pLinePort;

    QPushButton*  m_pButtonConnect;
    QPushButton*  m_pButtonSend;
    QPushButton*  m_pButtonClose;

    QTextEdit*    m_pTextRead;
    QTextEdit*    m_pTextWrite;

    QTcpSocket*   m_pTcpSocket;

    bool          m_bConnect;
    QByteArray m_socket_buffer;
};

#endif
