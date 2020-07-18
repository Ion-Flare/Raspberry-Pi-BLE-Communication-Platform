#include "wittern.h"
#include "ui_wittern.h"
#include <QProcess>
#include <QPixmap>

#include "chatserver.h"
#include "chatclient.h"

#include <qbluetoothdeviceinfo.h>
#include <qbluetoothaddress.h>
#include <qbluetoothlocaldevice.h>
#include <qbluetoothuuid.h>
#include <qbluetoothserver.h>
#include <qbluetoothservicediscoveryagent.h>

#include <qbluetoothtransferrequest.h>
#include <qbluetoothtransferreply.h>
#include <qbluetoothlocaldevice.h>
#include <QTextStream>
#include <QFileInfo>
#include <QClipboard>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

#ifdef Q_OS_ANDROID
#include <QtAndroidExtras/QtAndroid>
#endif

#include <QTimer>

#include <QDebug>


static const QLatin1String serviceUuid("e8e10f95-1a70-4b27-9ccf-02010264e9c8");
#ifdef Q_OS_ANDROID
static const QLatin1String reverseUuid("c8e96402-0102-cf9c-274b-701a950fe1e8");
#endif


Wittern::Wittern(QWidget *parent) : QMainWindow(parent),currentAdapterIndex(0), ui(new Ui::Wittern)
{

   // FileTransfer();

    ui->setupUi(this);




    ui->background->setStyleSheet("background-color: black;");
   // ui->graphicsView->setStyleSheet("background-color: #ff9020;");
    //    ui->->setStyleSheet("background-color: #90ffff;");
    ui->UnlockButton->setStyleSheet("background-color: #90ffff;");
ui->quitButton->setStyleSheet("background-color: #90ffff;");
ui->Cancel->setStyleSheet("background-color: #90ffff;");
ui->sendButton->setStyleSheet("background-color: #90ffff");
ui->connectButton->setStyleSheet("background-color: #90ffff;");
ui->FlexUpdate->setStyleSheet("background-color: #90ffff;");
ui->Dex->setStyleSheet("background-color: #90ffff;");
ui->chat->setStyleSheet("background-color: black;");
//ui->status->setStyleSheet("text-color: white");

QPixmap pix(":/new/bluBack/SubMenuBackground.png");
ui->n1->setPixmap(pix);
ui->n2->setPixmap(pix);
ui->n3->setPixmap(pix);
ui->n4->setPixmap(pix);
ui->n5->setPixmap(pix);
ui->n6->setPixmap(pix);
ui->n7->setPixmap(pix);

    //connect(ui->quitButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(ui->connectButton, SIGNAL(clicked()), this, SLOT(connectClicked()));
    connect(ui->sendButton, SIGNAL(clicked()), this, SLOT(sendClicked()));
    //! [Construct UI]

    localAdapters = QBluetoothLocalDevice::allDevices();
    if (localAdapters.count() < 2) {
        ui->localAdapterBox->setVisible(false);
    } else {
        //we ignore more than two adapters
        ui->localAdapterBox->setVisible(true);
        ui->firstAdapter->setText(tr("Default (%1)", "%1 = Bluetooth address").
                                  arg(localAdapters.at(0).address().toString()));
        ui->secondAdapter->setText(localAdapters.at(1).address().toString());
        ui->firstAdapter->setChecked(true);
        connect(ui->firstAdapter, SIGNAL(clicked()), this, SLOT(newAdapterSelected()));
        connect(ui->secondAdapter, SIGNAL(clicked()), this, SLOT(newAdapterSelected()));
        QBluetoothLocalDevice adapter(localAdapters.at(0).address());
        adapter.setHostMode(QBluetoothLocalDevice::HostDiscoverable);
    }

    //! [Create Chat Server]
    server = new ChatServer(this);
    connect(server, SIGNAL(clientConnected(QString)), this, SLOT(clientConnected(QString)));
    connect(server, SIGNAL(clientDisconnected(QString)), this, SLOT(clientDisconnected(QString)));
    connect(server, SIGNAL(messageReceived(QString,QString)),
            this, SLOT(showMessage(QString,QString)));
    connect(this, SIGNAL(sendMessage(QString)), server, SLOT(sendMessage(QString)));
    server->startServer();
    //! [Create Chat Server]

    //! [Get local device name]
    localName = QBluetoothLocalDevice().name();
    //! [Get local device name]
    //!
}

Wittern::~Wittern()
{
    delete ui;
    delete m_discoveryAgent;
    //qDeleteAll(clients);
    delete server;
}


//! [clientConnected clientDisconnected]
void Wittern::clientConnected(const QString &name)
{
    //ui->chat->insertPlainText( QFile("/home/pi/Desktop/7floorWTN.txt");    // put dex file address here
(QString::fromLatin1("%1 connected.\n").arg(name));
}


void Wittern::clientDisconnected(const QString &name)
{
    ui->chat->insertPlainText(QString::fromLatin1("%1 disconnected.\n").arg(name));
}
//! [clientConnected clientDisconnected]

//! [connected]
void Wittern::connected(const QString &name)
{
    ui->chat->insertPlainText(QString::fromLatin1("Linked to %1.\n").arg(name));
}

void Wittern::NewAdapterSelected(){
    const int newAdapterIndex = adapterFromUserSelection();
    if (currentAdapterIndex != newAdapterIndex) {
        server->stopServer();
        currentAdapterIndex = newAdapterIndex;
        const QBluetoothHostInfo info = localAdapters.at(currentAdapterIndex);
        QBluetoothLocalDevice adapter(info.address());
        adapter.setHostMode(QBluetoothLocalDevice::HostDiscoverable);
        server->startServer(info.address());
        localName = info.name();

;
    }

}

int Wittern::adapterFromUserSelection() const
{
    int result = 0;
    QBluetoothAddress newAdapter = localAdapters.at(0).address();

    if (ui->secondAdapter->isChecked()) {
        newAdapter = localAdapters.at(1).address();
        result = 1;
    }
    return result;
}
//! [connected]

//! [clientDisconnected]
void Wittern::clientDisconnected()
{
    ChatClient *client = qobject_cast<ChatClient *>(sender());
    if (client) {
        clients.removeOne(client);
        client->deleteLater();
    }
}




//! [clientDisconnected]
//___________________________________________________________________________________________________________________________
void Wittern::connectClicked(){

    ui->connectButton->setEnabled(false);

    // scan for services
    const QBluetoothAddress adapter = localAdapters.isEmpty() ?
                                          QBluetoothAddress() :
                                          localAdapters.at(currentAdapterIndex).address();

                    RemoteSelector(adapter);

#ifdef Q_OS_ANDROID
    if (QtAndroid::androidSdkVersion() >= 23)
        startDiscovery(QBluetoothUuid(reverseUuid));
    else
        startDiscovery(QBluetoothUuid(serviceUuid));
#else
   startDiscovery(QBluetoothUuid(serviceUuid));
#endif

   if(Grouper==1/*RemoteSelector.exec ==QDialog::Accepted*/){
       QBluetoothServiceInfo service = m_service;
       qDebug() << "Connecting to service 2" << service.serviceName()
                << "on" << service.device().name();

       // Create client
       qDebug() << "Going to create client";
       ChatClient *client = new ChatClient(this);
qDebug() << "Connecting...";

connect(client, SIGNAL(messageReceived(QString,QString)),
        this, SLOT(showMessage(QString,QString)));
connect(client, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));
connect(client, SIGNAL(connected(QString)), this, SLOT(connected(QString)));
connect(this, SIGNAL(sendMessage(QString)), client, SLOT(sendMessage(QString)));
qDebug() << "Start client";
client->startClient(service);

clients.append(client);
}
    ui->connectButton->setEnabled(true);

}
//____________________________________________________________________________________________________________________________


void Wittern::RemoteSelector(const QBluetoothAddress &localAdapter){

    m_discoveryAgent = new QBluetoothServiceDiscoveryAgent(localAdapter);

    connect(m_discoveryAgent, SIGNAL(serviceDiscovered(QBluetoothServiceInfo)),
            this, SLOT(serviceDiscovered(QBluetoothServiceInfo)));
    connect(m_discoveryAgent, SIGNAL(finished()), this, SLOT(discoveryFinished()));
    connect(m_discoveryAgent, SIGNAL(canceled()), this, SLOT(discoveryFinished()));
}

QBluetoothServiceInfo Wittern::service() const
{
    return m_service;
}

void Wittern::startDiscovery(const QBluetoothUuid &uuid)
{
    ui->status->setText(tr("Scanning..."));
    if (m_discoveryAgent->isActive())
        m_discoveryAgent->stop();

    ui->remoteDevices->clear();

    m_discoveryAgent->setUuidFilter(uuid);
    m_discoveryAgent->start(QBluetoothServiceDiscoveryAgent::FullDiscovery);

}


void Wittern::stopDiscovery()
{
    if (m_discoveryAgent){
        m_discoveryAgent->stop();
    }
}


void Wittern::serviceDiscovered(const QBluetoothServiceInfo &serviceInfo)
{
#if 0
    qDebug() << "Discovered service on"
             << serviceInfo.device().name() << serviceInfo.device().address().toString();
    qDebug() << "\tService name:" << serviceInfo.serviceName();
    qDebug() << "\tDescription:"
             << serviceInfo.attribute(QBluetoothServiceInfo::ServiceDescription).toString();
    qDebug() << "\tProvider:"
             << serviceInfo.attribute(QBluetoothServiceInfo::ServiceProvider).toString();
    qDebug() << "\tL2CAP protocol service multiplexer:"
             << serviceInfo.protocolServiceMultiplexer();
    qDebug() << "\tRFCOMM server channel:" << serviceInfo.serverChannel();
#endif
    QMapIterator<QListWidgetItem *, QBluetoothServiceInfo> i(m_discoveredServices);
    while (i.hasNext()){
        i.next();
        if (serviceInfo.device().address() == i.value().device().address()){
            return;
        }
    }

    QString remoteName;
    if (serviceInfo.device().name().isEmpty())
        remoteName = serviceInfo.device().address().toString();
    else
        remoteName = serviceInfo.device().name();

    QListWidgetItem *item =
        new QListWidgetItem(QString::fromLatin1("%1 %2").arg(remoteName,serviceInfo.serviceName()));

    m_discoveredServices.insert(item, serviceInfo);
    ui->remoteDevices->addItem(item);
}

void Wittern::discoveryFinished()
{
    ui->status->setText(tr("Devices connecting"));
}

void Wittern::on_remoteDevices_itemActivated(QListWidgetItem *item)
{
    qDebug() << "got click" << item->text();
   m_service = m_discoveredServices.value(item);
   if (m_discoveryAgent->isActive())
       m_discoveryAgent->stop();
Grouper=1;
   }

void Wittern::on_Cancel_clicked()  //cancel button
{
 Grouper=0;
 clientDisconnected();
}

//! [sendClicked]
void Wittern::sendClicked()
{
    ui->sendButton->setEnabled(false);
    ui->sendText->setEnabled(false);

    showMessage(localName, ui->sendText->text());
    emit sendMessage(ui->sendText->text());

    ui->sendText->clear();

    ui->sendText->setEnabled(true);
    ui->sendButton->setEnabled(true);




}
//! [sendClicked]
//! [showMessage]
void Wittern::showMessage(const QString &sender, const QString &message)
{
    ui->chat->insertPlainText(QString::fromLatin1("%1: %2\n").arg(sender, message));
    ui->chat->ensureCursorVisible();

    //sends dex file after a request is sent

   if (message == "Dex File?"){
    //ui->chat->insertPlainText(QString::fromLatin1("%1: Request for Dex File Sent.\n").arg(sender));
    ui->chat->insertPlainText(QString::fromLatin1("%1: Sending.\n").arg(sender));
      FileSend();
    ui->chat->insertPlainText(QString::fromLatin1("%1: Dex File Sent...\n").arg(sender));
    }


}
   //! [showMessage]



void Wittern::FileSend(){
    qDebug() << "Sending dex file";

    QBluetoothTransferManager mgr;
    QBluetoothTransferRequest req(m_service.device().address());

    m_file = new QFile("/home/pi/Desktop/7floorWTN.txt");    // put dex file address here
        QBluetoothTransferReply *reply = mgr.put(req,m_file);
        reply->setParent(this);



    QFile file("/home/pi/Desktop/7floorWTN.txt");
   if(file.open(QFile::ReadOnly |QFile::Text)){
       QTextStream in(&file);

       ui->sendText->insert(in.readAll());  //puts dex file into type bar


       ui->sendButton->setEnabled(false);   //sends file data
       ui->sendText->setEnabled(false);\


       showMessage("", ui->sendText->text());
       emit sendMessage(ui->sendText->text());
       ui->sendText->clear();

       ui->sendText->setEnabled(true);\
       ui->sendButton->setEnabled(true);

   }


}



void Wittern::on_Dex_clicked(){

    QString message= QString::fromLatin1("Dex File?");
    showMessage(localName,message);
emit sendMessage(message);

}

//! [Requests a dex file from a vending machine]


//! [Update Vending Machine]

void Wittern::on_FlexUpdate_clicked(){

    QString message= QString::fromLatin1("\n  Update processing...\n");
 showMessage(localName, message);
 //perform update
 m_file = new QFile("/home/wittern/Desktop/SBCVending");    // put flex address here
 QBluetoothTransferManager mgr;
 QBluetoothTransferRequest req(m_service.device().address());
 QBluetoothTransferReply *send = mgr.put(req,m_file);

 send->setParent(this);

 showMessage(("     System"),("Update Sent \n"));
 emit sendMessage("Update Sent \n");
}

//! [Update Vending Machine]


/*
void Wittern::on_LockButton_clicked()
{
    ui->chat->setTextColor(QColor(Qt::green));
        ui->chat->setText("\n pi@raspberrypi:~ $");
        ui->chat->setTextColor(QColor(Qt::white));
    QProcess lock;
    lock.start("/bin/bash",QStringList()<<"-c"<<"sudo python /home/pi/WitternVend/pyth_scrips/gatty_lock.py 00:1B:C5:01:4A:FC");
    lock.waitForFinished(-1);
    QString out1=lock.readAll();
    ui->chat->setText(out1);
}
*/

void Wittern::on_UnlockButton_clicked()
{
    ui->chat->setTextColor(QColor(Qt::green));
            ui->chat->setText("\n pi@raspberrypi:~ $");
            ui->chat->setTextColor(QColor(Qt::white));

    QProcess unlock;
    unlock.start("/bin/bash",QStringList()<<"-c"<<"sudo python /home/pi/WitternVend/pyth_scrips/gatty_unlock.py 00:1B:C5:01:4A:FC");
    unlock.waitForFinished(-1);

    QString out2=unlock.readAll();
    ui->chat->setText(out2);
}
