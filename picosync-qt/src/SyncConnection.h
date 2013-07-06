#ifndef SYNCCONNECTION_H
#define SYNCCONNECTION_H

#include "Discovery.h"
#include "utp/Connection.h"

class SyncConnection: public QObject {
	Q_OBJECT
private:
	Utp::Connection mUtp;
	Discovery::Announce mAnnounce;

	QByteArray mLocalNonce, mRemoteNonce;
	QByteArray mEncryptionKey, mKeyHash;

	/**
	 * \brief Takes the localNonce, remoteNonce and mAnnounce's R/O secret
	 *	and calculates the encryption key as well as the keyHash
	 * \param localNonce The random 32byte string sent by the initiating peer
	 * \param remoteNonce Random 32byte string sent by the other peer
	 * \param keyHash Output parameter, will be filled with the key hash (=sha256(encryptionKey))
	 * \returns Encryption key based on the nonce strings and the R/O secret
	 */
	QByteArray _getEncryptionKey(const QByteArray &localNonce, const QByteArray &remoteNonce, QByteArray *keyHash);
public:
	SyncConnection(const Discovery::Announce &announce, quint16 localPort);

	void connect(const QHostAddress &host, quint16 port);

signals:
	void gotDataFrame(const QByteArray &data);
private slots:
	void _gotData(const QByteArray &data);
	void _utpConnected();
};

#endif // SYNCCONNECTION_H
