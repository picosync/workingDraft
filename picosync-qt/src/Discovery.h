#ifndef DISCOVERY_H
#define DISCOVERY_H

#include "Peer.h"
#include "PeerAddress.h"
#include "Secret.h"

#include <QByteArray>
#include <QHostAddress>
#include <QObject>
#include <QUdpSocket>

class Discovery: public QObject {
	Q_OBJECT
public:
	enum MessageType {
		UnknownType = 0,
		PeerAnnounceType = 1,
		TrackerAnnounceType = 4,
		TrackerResponseType = 5
	};

	/**
	 * \brief Represents an Announce message
	 * \immutable
	 *
	 * Announce messages consist of the sender's Peer ID + address and the
	 * Secret of the corresponding share.
	 *
	 * They are sent to as tracker request packets and directly between peers
	 * \note
	 */
	class Announce {
	private:
		/// Secret to announce
		Secret mSecret;

		/// Sender's Peer ID+address
		Peer mPeer;
	public:
		Announce(const Secret &secret, const Peer &peer);
		Announce();

		/**
		 * \brief Returns the Peer of this Announce
		 * \returns Peer (make sure to check Peer::isValid())
		 */
		Peer getPeer() const {
			return mPeer;
		}

		/**
		 * \param Returns the Secret
		 * \returns Secret (make sure to check Secret::isValid())
		 */
		Secret getSecret() const {
			return mSecret;
		}

		/**
		 * \brief Check if the Announce object is valid
		 * \returns getPeer().isValid() && getSecret().isValid()
		 */
		bool isValid() const {
			return mPeer.isValid() && mSecret.isValid();
		}

		/**
		 * \brief Returns a binary request as it will be sent to the tracker/to other peers
		 * \param localAddress Local IPv4 address (which will be sent along with the packet)
		 * \param port Port we're listening at locally (will also be put into the packet)
		 * \param mode Message type
		 * \returns bencoded representation of the request packet (prefixed with a "BSYNC\0")
		 * \note This method will ignore the local address stored in getPeer().getLocalAddress() and use the supplied parameters instead
		 */
		QByteArray toBinary(const QHostAddress &localAddress, quint16 port, MessageType mode = TrackerAnnounceType) const;

		/**
		 * \brief Parses a binary request packet into an Announce object
		 * \param data Binary request data
		 * \returns Announce object (might be invalid!)
		 */
		static Announce fromBinary(const QByteArray &data);
	};

	/**
	 * \brief Tracker response message
	 * \immutable
	 *
	 * TrackerResponse objects contain the information that was received from the Tracker
	 * server after sending an announce packet to it.
	 *
	 * They contain a list of Peers that are connected to the same share, the
	 * external address of the local peer.
	 */
	class TrackerResponse {
	private:
		MessageType mType;
		QList<Peer> mPeers;
		PeerAddress mExternalAddress;
		QByteArray mShareHash;
	public:
		TrackerResponse(MessageType type, const QList<Peer> peers, const PeerAddress &externalAddress, const QByteArray &shareHash);
		TrackerResponse();

		bool isValid() const {
			return mType != UnknownType;
		}

		static TrackerResponse fromBinary(const QByteArray &data);
	};

private:
	QUdpSocket mSock;

public:
	Discovery(const QHostAddress &addr, quint16 port);

	void sendTrackerRequest(const Announce &announce);
	void sendPeerAnnounce(const Announce &announce, const QHostAddress &ip, quint16 port);

	void sendRawPkg(const QByteArray &msg, const QHostAddress &recipient, qint16 port);
private slots:
	void _gotPacket();

signals:
	void gotPacket(const QByteArray &pkg, const QHostAddress &sender, quint16 senderPort);
	void gotTrackerResponse(const TrackerResponse &resp);
	void gotPeerResponse(const Discovery::Announce &resp, const QHostAddress &host, quint16 port);

};

#endif // DISCOVERY_H
