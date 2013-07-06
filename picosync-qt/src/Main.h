#ifndef MAIN_H
#define MAIN_H

#include "Discovery.h"
#include "SyncConnection.h"

#include <QMap>
#include <QObject>
#include <QtCrypto/QtCrypto>

class Main: public QObject {
	Q_OBJECT
private:
	QCA::Initializer mQcaInit;
	Discovery mDiscovery;
	QMap<Peer, SyncConnection*> mConnections;
	Discovery::Announce mAnnounce;

public:
	explicit Main(QObject *parent = 0);
	~Main();

	void run();

private slots:
	void _gotPeerResponse(const Discovery::Announce &response, const QHostAddress &host, quint16 port);
	void _discoveryGotPacket(const QByteArray &pkg, const QHostAddress &sender, quint16 senderPort);

};

#endif // MAINWINDOW_H
