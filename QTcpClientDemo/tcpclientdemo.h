#ifndef TCPCLIENTDEMO_H
#define TCPCLIENTDEMO_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QTcpSocket>

class TcpClientDemo : public QWidget
{
    Q_OBJECT

public:
    TcpClientDemo(QWidget *parent = 0);
    ~TcpClientDemo();

private slots:
    void BtnConnectClicked();
    void BtnSendClicked();
    void BtnCloseClicked();

    void SocketConnected();
    void SocketDisconnected();
    void SocketReadyRead();

private:
    void InitClientLayout();
    void InitClientConnect();
    QString GetCurrentTime();

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
};

#endif // TCPCLIENTDEMO_H
