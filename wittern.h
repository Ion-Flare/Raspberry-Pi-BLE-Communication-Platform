#ifndef WITTERN_H
#define WITTERN_H

#include <QMainWindow>
#include <QDialog>
#include <QTime>
#include <qbluetoothuuid.h>
#include <qbluetoothserviceinfo.h>
#include <qbluetoothservicediscoveryagent.h>
#include <qbluetoothsocket.h>
#include <qbluetoothhostinfo.h>


#include <QPointer>
#include <qbluetoothlocaldevice.h>


#include <QWidget>
#include <QDebug>

QT_FORWARD_DECLARE_CLASS(QModelIndex)
QT_FORWARD_DECLARE_CLASS(QListWidgetItem)
QT_FORWARD_DECLARE_CLASS(QBluetoothTransferReply)
QT_FORWARD_DECLARE_CLASS(QFile)

class ChatServer;
class ChatClient;
//class fileTransfer;


namespace Ui {
class Wittern;
}

class Wittern : public QMainWindow
{
    Q_OBJECT

public:
    explicit Wittern(QWidget *parent = 0);
    ~Wittern();


    void startDiscovery(const QBluetoothUuid &uuid);
    void stopDiscovery();
    void RemoteSelector(const QBluetoothAddress &localAdapter);

    QBluetoothServiceInfo service() const;


int Grouper;


signals:
    void sendMessage(const QString &message);
    //void SendD(QString &message);


private slots:
    void discoveryFinished();
    void serviceDiscovered(const QBluetoothServiceInfo &serviceInfo);

   void on_remoteDevices_itemActivated(QListWidgetItem *item);
    void connectClicked();
    void sendClicked();
    void showMessage(const QString &sender, const QString &message);

    void clientConnected(const QString &name);
    void clientDisconnected(const QString &name);
    void clientDisconnected();
    void connected(const QString &name);
        void NewAdapterSelected();


    void on_Dex_clicked();
        void on_FlexUpdate_clicked();
     void on_Cancel_clicked();  //cancel button

  void FileSend();

  //void on_LockButton_clicked();

  void on_UnlockButton_clicked();

private:
   int adapterFromUserSelection() const;
   int currentAdapterIndex;

   Ui::Wittern *ui;

    ChatServer *server;
    QList<ChatClient *> clients;
    QList<QBluetoothHostInfo> localAdapters;
    QString localName;

    QBluetoothServiceDiscoveryAgent *m_discoveryAgent;
    QBluetoothServiceInfo m_service;
    QMap<QListWidgetItem *, QBluetoothServiceInfo> m_discoveredServices;

QTime start;
    QFile *m_file;
};

#endif // WITTERN_H
