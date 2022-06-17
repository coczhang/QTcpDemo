#include "tcpserverdemo.h"

#include <QDateTime>

TcpServerDemo::TcpServerDemo(QWidget *parent)
    : QWidget(parent)
{
    InitClientLayout();
    InitClientConnect();
}

TcpServerDemo::~TcpServerDemo()
{
    if(m_pTcpSocket->isOpen())
    {
        m_pTcpSocket->disconnectFromHost();
        m_pTcpSocket->close();
    }

    if (m_pTcpSocket != NULL)
    {
        delete m_pTcpSocket;
        m_pTcpSocket = NULL;
    }

    //主动和客户端端断开连接
    if(m_pTcpServer != NULL)
    {
        delete m_pTcpSocket;
        m_pTcpSocket = NULL;
    }
}

void TcpServerDemo::InitClientLayout()
{
    this->setWindowTitle("服务端");
    this->setFixedSize(this->width(), this->height());

    m_pTextRead = new QTextEdit(this);
    m_pTextRead->setGeometry(10, 5, this->width() - 20, this->height() / 2 - 40);
    m_pTextRead->setReadOnly(true);

    m_pTextWrite = new QTextEdit(this);
    m_pTextWrite->setGeometry(10, this->height() / 2 - 30, this->width() - 20, this->height() / 2 - 40);

    m_pButtonSend = new QPushButton("Send", this);
    m_pButtonSend->setGeometry(10, this->height() - 50, 80, 30);

    m_pButtonClose = new QPushButton("Close", this);
    m_pButtonClose->setGeometry(this->width() - 90, this->height() - 50, 80, 30);
}

void TcpServerDemo::InitClientConnect()
{
    //监听套接字
    m_pTcpServer = new QTcpServer(this); //指定父对象，让Qt自动回收内存控件
    m_pTcpServer->listen(QHostAddress::Any, CONNECT_PORT);

    connect(m_pTcpServer, &QTcpServer::newConnection, this, &TcpServerDemo::AcceptSocketConnect);

    connect(m_pButtonSend, &QPushButton::clicked, this, &TcpServerDemo::BtnSendClicked);
    connect(m_pButtonClose, &QPushButton::clicked, this, &TcpServerDemo::BtnCloseClicked);
}

QString TcpServerDemo::GetCurrentTime()
{
    QTime time = QTime::currentTime();
    return (QString("[当前时间:%1:%2:%3]").arg(time.hour()).arg(time.minute()).arg(time.second()));
}

void TcpServerDemo::BtnSendClicked()
{
    if (NULL == m_pTcpSocket)
    {
        QString str = QString("没有客户端与服务端连接......%1").arg(GetCurrentTime());
        m_pTextRead->append(str);
        return;
    }
    QString strText = m_pTextWrite->toPlainText();
    //给对方发送数据，使用套接字tcpSocket
    m_pTcpSocket->write(strText.toUtf8().data());
}

void TcpServerDemo::BtnCloseClicked()
{
    if(NULL == m_pTcpSocket)
    {
        QString str = QString("没有客户端与服务端连接......%1").arg(GetCurrentTime());
        m_pTextRead->append(str);
        return;
    }

    //主动和客户端端断开连接
    if(m_pTcpSocket->isOpen())
    {
        m_pTcpSocket->disconnectFromHost();
        m_pTcpSocket->close();
    }

    if (m_pTcpSocket != NULL)
    {
        delete m_pTcpSocket;
        m_pTcpSocket = NULL;
    }
}

void TcpServerDemo::AcceptSocketConnect()
{
    QString strDisplay;
    //取出建立好链接的套接字
    m_pTcpSocket = m_pTcpServer->nextPendingConnection();
    if (NULL == m_pTcpSocket)
    {
        strDisplay = QString("客户端连接失败%3").arg(GetCurrentTime());
        m_pTextRead->append(strDisplay);
        return;
    }

    //获取对方的IP和端口
    QString strIP = m_pTcpSocket->peerAddress().toString();
    quint16 nPort = m_pTcpSocket->peerPort();
    strDisplay = QString("[%1:%2]:连接成功%3").arg(strIP).arg(nPort).arg(GetCurrentTime());
    m_pTextRead->append(strDisplay);

    connect(m_pTcpSocket, &QTcpSocket::readyRead, this, &TcpServerDemo::SocketReadyRead);
    connect(m_pTcpSocket, &QTcpSocket::disconnected, this, &TcpServerDemo::SocketDisconnected);
}

void TcpServerDemo::SocketDisconnected()
{
    //主动和客户端端断开连接
    if(m_pTcpSocket->isOpen())
    {
        m_pTcpSocket->disconnectFromHost();
        m_pTcpSocket->close();

        QString str =QString("客户端断开连接......%1").arg(GetCurrentTime());
        m_pTextRead->append(str);
    }

    if (m_pTcpSocket != NULL)
    {
        delete m_pTcpSocket;
        m_pTcpSocket = NULL;
    }

}

void TcpServerDemo::SocketReadyRead()
{
    //从通信套接字中取出内容
    QByteArray array = m_pTcpSocket->readAll();
    QString str = QString("%1 %2").arg(QString(array)).arg(GetCurrentTime());
    m_pTextRead->append(str);
}
