#include "Discovery.h"
#include "PeerAddress.h"

#include <QHostInfo>
#include <QStringList>
#include <libtorrent/bencode.hpp>
#include <libtorrent/entry.hpp>
#include <map>
#include <vector>

const QStringList trackerHosts = QStringList() << "r.usyncapp.com" << "t.usyncapp.com";

#define TRACKER_PORT 3000

Discovery::Announce::Announce(const Secret &secret, const Peer &peer): mSecret(secret), mPeer(peer) {
}

Discovery::Announce::Announce() {
}

Discovery::Announce Discovery::Announce::fromBinary(const QByteArray &data) {
	Announce rc;

	if (data.startsWith(QByteArray("BSYNC\0", 6))) {
		libtorrent::entry e = libtorrent::bdecode(data.begin()+6, data.end());
		libtorrent::entry::dictionary_type dict = e.dict();

		MessageType m = (MessageType) dict.at("m").integer();
		std::string peerId = dict.at("peer").string();
		std::string shareHash = dict.at("share").string();
		PeerAddress localAddress, externalAddress;
		if (dict.count("la")) {
			localAddress = PeerAddress::fromBinary(dict.at("la").string());
		}
		if (dict.count("a")) {
			externalAddress = PeerAddress::fromBinary(dict.at("a").string());
		}


		rc = Announce(Secret::fromShareHash(shareHash), Peer(QByteArray(peerId.data(), peerId.length()), localAddress, externalAddress));
	}

	return rc;
}

QByteArray Discovery::Announce::toBinary(const QHostAddress &localAddress, quint16 port, MessageType mode) const {
	std::vector<char> buffer;
	libtorrent::entry::dictionary_type data;

	data.insert(std::make_pair("share", mSecret.getShareHashStdString()));
	data.insert(std::make_pair("peer", mPeer.getIdBinaryStdString()));
	data.insert(std::make_pair("la", PeerAddress(localAddress, port).getBinaryAddressStdString()));
	data.insert(std::make_pair("m", mode));

	libtorrent::bencode(std::back_inserter(buffer), data);

	std::string str(buffer.begin(), buffer.end());
	QByteArray rc;

	rc.reserve(str.length()+6);
	rc.append("BSYNC\0", 6);
	rc.append(str.data(), str.length());

	return rc;
}

Discovery::TrackerResponse::TrackerResponse(MessageType type, const QList<Peer> peers, const PeerAddress &externalAddress, const QByteArray &shareHash) {
	mType = type;
	mPeers = peers;
	mExternalAddress = externalAddress;
	mShareHash = shareHash;
}

Discovery::TrackerResponse::TrackerResponse() {
	mType = UnknownType;
}

Discovery::TrackerResponse Discovery::TrackerResponse::fromBinary(const QByteArray &data) {
	MessageType type = UnknownType;
	QList<Peer> peers;
	TrackerResponse rc;

	if (data.startsWith("BSYNC\0")) {
		libtorrent::entry e = libtorrent::bdecode(data.begin()+6, data.end());

		libtorrent::entry::dictionary_type dict = e.dict();

		try {
			type = (MessageType) dict.at("m").integer();
			if (dict.count("peers")) {
				libtorrent::entry::list_type stdPeers = dict.at("peers").list();


			}
			PeerAddress extAddr = PeerAddress::fromBinary(dict.at("ea").string());
			std::string shareHash = dict.at("share").string();
			rc = TrackerResponse(type, peers, extAddr, QByteArray(shareHash.data(), shareHash.length()));
		}
		catch (std::out_of_range e) {

		}
	}

	return rc;
}

Discovery::Discovery(const QHostAddress &addr, quint16 port) {
	connect(&mSock, SIGNAL(readyRead()), this, SLOT(_gotPacket()));

	mSock.bind(addr, port);
}

void Discovery::_gotPacket() {
	QHostAddress address;
	QByteArray buff;
	quint16 port;

	while (mSock.hasPendingDatagrams()) {
		qint64 size = mSock.pendingDatagramSize();
		buff.reserve(size);

		buff.resize(mSock.readDatagram(buff.data(), size, &address, &port));

		if (buff.startsWith("BSYNC\0")) {
			/// @todo Use some other means to distinguish between tracker TrackerResponse and Announce
			if (port == 3000) {
				TrackerResponse resp = TrackerResponse::fromBinary(buff);
				emit gotTrackerResponse(resp);
			}
			else {
				Announce req = Announce::fromBinary(buff);
				emit gotPeerResponse(req);
			}
		}
		else {

		}
		emit gotPacket(buff, address, port);
	}
}

void Discovery::sendPeerAnnounce(const Announce &announce, const QHostAddress &ip, quint16 port) {
	mSock.writeDatagram(announce.toBinary(mSock.localAddress(), mSock.localPort(), PeerAnnounceType), ip, port);
}

void Discovery::sendRawPkg(const QByteArray &msg, const QHostAddress &recipient, qint16 port) {
	mSock.writeDatagram(msg, recipient, port);
}

void Discovery::sendTrackerRequest(const Announce &announce) {
	/// @todo Use r.usyncapp.com and t.usyncapp.com (and possibly some custom trackers)
	//QHostAddress target = QHostInfo::fromName(trackerHosts.last()).addresses().first();
	QHostAddress target("54.225.100.8");

	mSock.writeDatagram(announce.toBinary(mSock.localAddress(), mSock.localPort()), target, TRACKER_PORT);
}
