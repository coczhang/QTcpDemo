#include "tcpclientdemo.h"

#include <QDateTime>
#include <QHostAddress>

TcpClientDemo::TcpClientDemo(QWidget *parent)
    : QWidget(parent)
{
    InitClientLayout();
    InitClientConnect();
}

TcpClientDemo::~TcpClientDemo()
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
}

void TcpClientDemo::InitClientLayout()
{
    this->setWindowTitle("客户端");
    this->setFixedSize(this->width(), this->height());

    m_pLabelIp = new QLabel("服务器IP：", this);
    m_pLabelIp->setGeometry(20, 10, 60, 30);

    m_pLineIp = new QLineEdit("127.0.0.1", this);
    m_pLineIp->setGeometry(90, 13, 400, 30);
    m_pLineIp->setDisabled(true);

    m_pLabelPort = new QLabel("服务器端口：", this);
    m_pLabelPort->setGeometry(20, 50, 70 ,30);

    m_pLinePort = new QLineEdit("8888", this);
    m_pLinePort->setGeometry(90, 50, 400, 30);
    m_pLinePort->setDisabled(true);

    m_pButtonConnect = new QPushButton("Connect", this);
    m_pButtonConnect->setGeometry(520, 35, 80, 30);

    m_pTextRead = new QTextEdit(this);
    m_pTextRead->setGeometry(20, 100, 600, 150);
    m_pTextRead->setReadOnly(true);

    m_pTextWrite = new QTextEdit(this);
    m_pTextWrite->setGeometry(20, 260, 600, 150);

    m_pButtonSend = new QPushButton("Send", this);
    m_pButtonSend->setGeometry(10, this->height() - 40, 80, 30);

    m_pButtonClose = new QPushButton("Close", this);
    m_pButtonClose->setGeometry(this->width()- 90, this->height() - 40, 80, 30);
}

void TcpClientDemo::InitClientConnect()
{
    m_bConnect = false;

    m_pTcpSocket = new QTcpSocket(this);

    connect(m_pTcpSocket, &QTcpSocket::connected, this, &TcpClientDemo::SocketConnected);
    connect(m_pTcpSocket, &QTcpSocket::readyRead, this, &TcpClientDemo::SocketReadyRead);
    connect(m_pTcpSocket, &QTcpSocket::disconnected, this, &TcpClientDemo::SocketDisconnected);

    connect(m_pButtonConnect, &QPushButton::clicked, this, &TcpClientDemo::BtnConnectClicked);
    connect(m_pButtonSend, &QPushButton::clicked, this, &TcpClientDemo::BtnSendClicked);
    connect(m_pButtonClose, &QPushButton::clicked, this, &TcpClientDemo::BtnCloseClicked);
}

void TcpClientDemo::BtnConnectClicked()
{
    if(m_bConnect)
    {
        QString str = QString("已连接服务端......%1").arg(GetCurrentTime());
        m_pTextRead->append(str);

        return;
    }

    //获取服务器IP和端口
    QString strIP = m_pLineIp->text();
    quint16 nPort = (m_pLinePort->text()).toUInt();

    //主动和服务器建立连接
    //m_pTcpSocket->abort();
    m_pTcpSocket->connectToHost(QHostAddress(strIP), nPort);
}

void TcpClientDemo::BtnSendClicked()
{
    if(!m_bConnect)
    {
        QString str = QString("请先连接服务端......%1").arg(GetCurrentTime());
        m_pTextRead->append(str);

        return;
    }


    //获取编辑框内容
    QString strText = m_pTextWrite->toPlainText();
    //发送数据
    m_pTcpSocket->write(strText.toUtf8().data());
}

void TcpClientDemo::BtnCloseClicked()
{
    if (!m_bConnect)
    {
        QString str = QString("服务端未连接......%1").arg(GetCurrentTime());
        m_pTextRead->append(str);

        return;
    }

    if(m_pTcpSocket->isOpen())
    {
        m_pTcpSocket->disconnectFromHost();
        m_pTcpSocket->close();

        m_bConnect = false;
        QString str = QString("客户端断开连接......%1").arg(GetCurrentTime());
        m_pTextRead->append(str);
    }
}

void TcpClientDemo::SocketConnected()
{
    m_bConnect = true;
    QString str = QString("成功和服务端建立连接......%1").arg(GetCurrentTime());
    m_pTextRead->append(str);
}

void TcpClientDemo::SocketDisconnected()
{
    if(m_pTcpSocket->isOpen())
    {
        m_pTcpSocket->disconnectFromHost();
        m_pTcpSocket->close();

        m_bConnect = false;
        QString str = QString("客户端断开连接......%1").arg(GetCurrentTime());
        m_pTextRead->append(str);
    }
}

void TcpClientDemo::SocketReadyRead()
{
    //获取对方发送的内容
    QByteArray array = m_pTcpSocket->readAll();
    QString str = QString("%1 %2").arg(QString(array)).arg(GetCurrentTime());
    //添加到编辑区中
    m_pTextRead->append(str);
}

QString TcpClientDemo::GetCurrentTime()
{
    QTime time = QTime::currentTime();
    return (QString("[当前时间:%1:%2:%3]").arg(time.hour()).arg(time.minute()).arg(time.second()));
}
