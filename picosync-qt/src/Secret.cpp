#include "Secret.h"
#include "Util.h"

#include <QFile>
#include <QtCrypto/QtCrypto>

Secret::Secret(const QByteArray &secret) {
	_init(secret);
}

Secret::Secret(const char *secret) {
	_init(secret);
}

Secret::Secret() {
}

void Secret::_init(const QByteArray &secret) {
	QCA::Hash sha("sha256");

	if (secret.startsWith('R')) {
		mRoSecret = secret;
		mType = RoSecretType;
	}
	else {
		mSecret = secret;

		// normal secret => hash twice
		sha.update(Util::base32decode(secret));
		mRoSecret = "R"+Util::base32encode(sha.final().toByteArray()).left(32);
		//qDebug("RO secret: %s", data.constData());
		sha.clear();
		mType = SecretType;
	}

	// hash the r/o secret again (without the leading 'R')
	sha.update(Util::base32decode(mRoSecret.mid(1)));
	mShareHash = sha.final().toByteArray();
}

Secret Secret::fromShareHash(const QByteArray &shareHash) {
	Secret rc;

	switch (shareHash.length()) {
	case 32:
		rc.mShareHash = shareHash;
		break;
	case 64:
		rc.mShareHash = QByteArray::fromHex(shareHash);
		break;
	default:
		qWarning("Secret::fromShareHash(): hash must be 32 bytes binary or 64 bytes hex, not %i bytes", shareHash.length());
	}

	return rc;
}

Secret Secret::generateRandomSecret() {
	QByteArray data = Util::getRandomData(20);
	Secret rc;

	if (data.length() >= 20) {
		rc = Secret(Util::base32encode(data));
	}

	return rc;
}

QByteArray Secret::getBinaryRoSecret() const {
	return Util::base32decode(mRoSecret.mid(1));
}

std::string Secret::getShareHashStdString() const {
	QByteArray hash = getShareHash();
	return std::string(hash.constData(), hash.length());
}
