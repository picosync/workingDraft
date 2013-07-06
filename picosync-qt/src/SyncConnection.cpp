#include "SyncConnection.h"
#include "Util.h"

#include <QtCrypto/QtCrypto>

SyncConnection::SyncConnection(const Discovery::Announce &announce, quint16 localPort): mUtp(localPort) {
	mAnnounce = announce;
	QObject::connect(&mUtp, SIGNAL(connected()), this, SLOT(_utpConnected()));
	QObject::connect(&mUtp, SIGNAL(gotData(QByteArray)), this, SLOT(_gotData(QByteArray)), Qt::QueuedConnection);
}

QByteArray SyncConnection::_getEncryptionKey(const QByteArray &rand1, const QByteArray &rand2, QByteArray *keyHash) {
	QCA::Hash sha("sha256");
	QByteArray rc;

	sha.update(rand1);
	sha.update(rand2);
	sha.update(mAnnounce.getSecret().getBinaryRoSecret());
	rc = sha.final().toByteArray();

	if (keyHash) {
		sha.clear();
		sha.update(rc);
		*keyHash = sha.final().toByteArray();
	}

	return rc;
}

void SyncConnection::_gotData(const QByteArray &data) {
	if (mLocalNonce.isEmpty()) {
		qWarning("Got data before we sent the nonce");
	}
	if (mRemoteNonce.isEmpty()) {
		if (data.length() == 38 && data.startsWith(QByteArray("BSYNC\0", 6))) {
			// receive nonce
			mRemoteNonce = data.mid(6);

			mEncryptionKey = _getEncryptionKey(mLocalNonce, mRemoteNonce, &mKeyHash);
			mUtp.send(mKeyHash);
		}
		else qWarning("Got unexpected sync data (len=%d): %s", data.length(), data.toHex().constData());
	}
	else if (!mKeyHash.isEmpty()) {
		if (data == mKeyHash) {
			qDebug("Established encrypted sync connection, that's as far as we can do this right now...");
		}
		else {
			qWarning("Unexpected challenge response!");
			qWarning("\tExpected: %s", mKeyHash.toHex().constData());
			qWarning("\tActual: %s", data.toHex().constData());
		}
		mKeyHash.clear(); // the next packet will end up in the else block
	}
	else {
		// From now on the data is split in chunks that start with their length
		qDebug("Got sync data (len=%d): %s", data.length(), data.toHex().constData());

		const char *pData = data.constData(), *pEnd = data.constData()+data.length();
		while (pEnd - pData >= 4) {
			unsigned frameLen = *reinterpret_cast<const int*>(pData);
			pData += 4;

			if (pEnd - pData >= frameLen) {
				QByteArray frame(pData, frameLen);
				qDebug("Received sync frame (len: %u): %s", frameLen, frame.toHex().constData());
				emit gotDataFrame(frame);

				pData += frameLen;
			}
			else qWarning("Invalid sync data: Frame (length: %u) exceeds remaining package length (%ld)", frameLen, pEnd-pData);
		}
	}
}

void SyncConnection::_utpConnected() {
	// Send the BSYNC initialization package
	qDebug("Sending BSYNC nonce...");
	mLocalNonce = Util::getRandomData(32);
	QByteArray pkg;
	pkg.reserve(38);
	pkg.append("BSYNC\0", 6);
	pkg.append(mLocalNonce);
	mUtp.send(pkg);
}

void SyncConnection::connect(const QHostAddress &host, quint16 port) {
	mUtp.connect(host, port);
}
