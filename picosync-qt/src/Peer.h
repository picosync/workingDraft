#ifndef PEER_H
#define PEER_H

#include "PeerAddress.h"

#include <QByteArray>
#include <QHostAddress>

class Peer {
private:
	/// Peer ID (binary, 20 bytes)
	QByteArray mPeerId;

	PeerAddress mExternalAddress;
	PeerAddress mLocalAddress;

	void _init(const QByteArray &peerId, const PeerAddress &localAddress, const PeerAddress &externalAddress);
public:
	/**
	 * \brief Constructor
	 * \param peerId Peer ID, can be either 20 bytes binary or 40 bytes hex
	 */
	Peer(const QByteArray &peerId);

	/**
	 * \brief Constructor
	 * \param peerId Peer ID, can be either 20 bytes binary or 40 bytes hex
	 * \param localAddress Peer's local address
	 * \param externalAddress Peer's external address
	 */
	Peer(const QByteArray &peerId, const PeerAddress &localAddress, const PeerAddress &externalAddress);

	/**
	 * \brief Default constructor, creates an invalid peer
	 */
	Peer();

	/**
	 * \brief Returns the Peer's local address
	 * \returns PeerAddress (might be invalid!)
	 */
	PeerAddress getLocalAddress() const {
		return mLocalAddress;
	}

	/**
	 * \brief Returns the Peer's external address
	 * \returns PeerAddress (might be invalid!)
	 */
	PeerAddress getExternalAddress() const {
		return mExternalAddress;
	}

	/**
	 * \brief Returns the binary peer id
	 * \returns Peer ID, binary, 20 bytes
	 */
	QByteArray getId() const {
		return mPeerId;
	}

	std::string getIdBinaryStdString() const;

	/**
	 * \brief Returns true if the peer id is valid
	 */
	bool isValid() const {
		return !mPeerId.isNull();
	}
};

#endif // PEER_H
