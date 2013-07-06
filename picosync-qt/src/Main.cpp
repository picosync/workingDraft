#include "Main.h"
#include "SyncConnection.h"

#include <QCoreApplication>

Main::Main(QObject *parent): QObject(parent), mDiscovery(QHostAddress("0.0.0.0"), 0) {
	connect(&mDiscovery, SIGNAL(gotPeerResponse(Discovery::Announce,QHostAddress,quint16)), this, SLOT(_gotPeerResponse(Discovery::Announce,QHostAddress,quint16)));
	connect(&mDiscovery, SIGNAL(gotPacket(QByteArray,QHostAddress,quint16)), this, SLOT(_discoveryGotPacket(QByteArray,QHostAddress,quint16)));
}

Main::~Main() {

}

void Main::_discoveryGotPacket(const QByteArray &pkg, const QHostAddress &sender, quint16 senderPort) {
	qDebug("Got packet from %s:%i (len: %i)", qPrintable(sender.toString()), senderPort, pkg.length());
}

void Main::_gotPeerResponse(const Discovery::Announce &response, const QHostAddress &host, quint16 port) {
	Peer peer = response.getPeer();
	if (!mConnections.contains(peer)) {
		qDebug("Found new peer:");
		qDebug("\tpeerId: %s", peer.getId().toHex().constData());
		if (peer.getLocalAddress().isValid()) {
			qDebug("\tAddress: %s:%i", qPrintable(peer.getLocalAddress().getAddress().toString()), peer.getLocalAddress().getPort());
		}
		else qDebug("\tAddress: (none)");
		qDebug("\tShare Hash: %s", response.getSecret().getShareHash().toHex().constData());

		qDebug("Opening connection...");
		SyncConnection *connection = new SyncConnection(mAnnounce, 0);
		connection->connect(host, port);

		mConnections.insert(peer, connection);
	}
}

void Main::run() {
	// find local peers
	QStringList args = QCoreApplication::arguments();

	Peer peer;
	if (args.length() < 2) {
		qWarning("Usage: %s <secret> [peerId]", qPrintable(args.at(0)));
		qWarning("Peer ID and Secret have to be base32 encoded");
		exit(1);
	}
	Secret secret(args.at(1).toAscii());

	if (args.length() > 2) {
		peer = Peer(args.at(2).toAscii());
	}
	else peer = Peer("12345678901234567890");

	mAnnounce = Discovery::Announce(secret, peer);
#if 0
	mDiscovery.sendPeerAnnounce(announce, QHostAddress("192.168.1.128"), 37714);
#else
	SyncConnection *conn = new SyncConnection(mAnnounce, 0);
	conn->connect(QHostAddress("192.168.1.128"), 37714);
#endif
}
