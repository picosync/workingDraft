#include "Peer.h"

Peer::Peer(const QByteArray &peerId) {
	_init(peerId, PeerAddress(), PeerAddress());
}

Peer::Peer(const QByteArray &peerId, const PeerAddress &address, const PeerAddress &localAddress) {
	_init(peerId, address, localAddress);
}

Peer::Peer() {
	_init(QByteArray(), PeerAddress(), PeerAddress());
}

void Peer::_init(const QByteArray &peerId, const PeerAddress &localAddress, const PeerAddress &externalAddress) {
	if (peerId.length() == 20) {
		mPeerId = peerId;
	}
	else if (peerId.length() == 40) {
		mPeerId = QByteArray::fromHex(peerId);
	}
	mLocalAddress = localAddress;
	mExternalAddress = externalAddress;
}

std::string Peer::getIdBinaryStdString() const {
	QByteArray id = getId();
	return std::string(id.constData(), id.length());
}
