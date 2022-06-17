#ifndef TCPSERVERDEMO_H
#define TCPSERVERDEMO_H

#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QTcpServer>
#include <QTcpSocket>

#define CONNECT_PORT    8888

class TcpServerDemo : public QWidget
{
    Q_OBJECT

public:
    TcpServerDemo(QWidget *parent = 0);
    ~TcpServerDemo();

private:
    void InitClientLayout();
    void InitClientConnect();

    QString GetCurrentTime();

private slots:
    void BtnSendClicked();
    void BtnCloseClicked();

    void AcceptSocketConnect();

    void SocketDisconnected();
    void SocketReadyRead();

private:
    QTextEdit*     m_pTextRead;
    QTextEdit*     m_pTextWrite;

    QPushButton*   m_pButtonSend;
    QPushButton*   m_pButtonClose;

    QTcpServer*    m_pTcpServer; //监听套接字
    QTcpSocket*    m_pTcpSocket; //通信套接字
};

#endif // TCPSERVERDEMO_H
