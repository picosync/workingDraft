#include "Main.h"

#include <QCoreApplication>

Main::Main(QObject *parent): QObject(parent), m_discovery(QHostAddress("192.168.1.24"), 3000) {
	connect(&m_discovery, SIGNAL(gotPeerResponse(Discovery::Announce)), this, SLOT(_gotPeerResponse(Discovery::Announce)));
	connect(&m_discovery, SIGNAL(gotPacket(QByteArray,QHostAddress,quint16)), this, SLOT(_discoveryGotPacket(QByteArray,QHostAddress,quint16)));
}

Main::~Main() {

}

void Main::_discoveryGotPacket(const QByteArray &pkg, const QHostAddress &sender, quint16 senderPort) {
	qDebug("Got packet from %s:%i (len: %i)", qPrintable(sender.toString()), senderPort, pkg.length());
}

void Main::_gotPeerResponse(const Discovery::Announce &response) {
	qDebug("Got peer response: %s", "todo");
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

	Discovery::Announce announce(secret, peer);
	m_discovery.sendPeerAnnounce(announce, QHostAddress("192.168.1.128"), 37714);
}
