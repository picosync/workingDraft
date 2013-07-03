#ifndef PEERADDRESS_H
#define PEERADDRESS_H

#include <QHostAddress>

/**
 * \brief Data class that represents a ip/port tuple and provides some useful methods on those
 * \immutable
 */
class PeerAddress {
private:
	/// ipv4 address
	QHostAddress mAddr;

	/// port number
	quint16 mPort;

public:
	/**
	 * \brief Constructor
	 * \param addr IPv4 address
	 * \param port UDP port
	 */
	PeerAddress(const QHostAddress &addr, quint16 port);

	/**
	 * \brief Default constructor, creates an invalid PeerAddress
	 * \see isValid()
	 */
	PeerAddress();

	/**
	 * \brief Returns a 6-byte binary representation of the local address and port
	 * \returns 4 bytes of the IPv4-Address and 2 bytes for the port
	 */
	std::string getBinaryAddressStdString();

	/**
	 * \brief Checks whether the PeerAddress object is valid
	 * \returns true if valid, false if invalid
	 */
	bool isValid() const {
		return !mAddr.isNull();
	}

	static inline PeerAddress fromBinary(const QByteArray &data) {
		return fromBinary(data.constData(), data.length());
	}

	static inline PeerAddress fromBinary(const std::string &data) {
		return fromBinary(data.data(), data.length());
	}

	/**
	 * \brief Parse a 6 byte binary ip/port pair (like it's sent in discovery messages)
	 * \param buff Data buffer
	 * \param len Data length (must be 6 or an invalid PeerAddress will be returned)
	 * \returns PeerAddress object
	 */
	static PeerAddress fromBinary(const char *buff, int len);
};

#endif // PEERADDRESS_H
