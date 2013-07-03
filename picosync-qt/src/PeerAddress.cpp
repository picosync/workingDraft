#include "PeerAddress.h"

#include <arpa/inet.h>

PeerAddress::PeerAddress(const QHostAddress &addr, quint16 port) {
	mAddr = addr;
	mPort = port;
}

PeerAddress::PeerAddress() {
	mPort = 0;
}

PeerAddress PeerAddress::fromBinary(const char *buff, int len) {
	PeerAddress rc;

	if (len == 6) {
		quint32 ip = ntohl(*((quint32*)buff));
		quint16 port = ntohs(*((quint16*) buff+4));
		rc = PeerAddress(QHostAddress(ip), port);
	}

	return rc;
}

std::string PeerAddress::getBinaryAddressStdString() {
	QByteArray rc;
	rc.reserve(6);
	qint32 ip = htonl(mAddr.toIPv4Address());
	quint16 port = htons(mPort);

	rc.append((char*) &ip, 4);
	rc.append((char*) &port, 2);

	return std::string(rc.constData(), rc.length());
}
