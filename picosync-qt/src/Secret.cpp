#include "Secret.h"
#include "cyoencode-1.0.2/src/CyoDecode.h"
#include "cyoencode-1.0.2/src/CyoEncode.h"

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
		sha.update(base32decode(secret));
		mRoSecret = "R"+base32encode(sha.final().toByteArray()).left(32);
		//qDebug("RO secret: %s", data.constData());
		sha.clear();
		mType = SecretType;
	}

	// hash the r/o secret again (without the leading 'R')
	sha.update(base32decode(mRoSecret.mid(1)));
	mShareHash = sha.final().toByteArray();
}

QByteArray Secret::base32decode(const QByteArray &in) {
	QByteArray rc;
	rc.reserve(cyoBase32DecodeGetLength(in.length()));

	rc.resize(cyoBase32Decode(rc.data(), in.constData(), in.length()));

	return rc;
}

QByteArray Secret::base32encode(const QByteArray &in) {
	QByteArray rc;
	rc.reserve(cyoBase32EncodeGetLength(in.length()));
	rc.resize(cyoBase32Encode(rc.data(), in.constData(), in.length()));

	return rc;
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
	QByteArray data;
	Secret rc;

#ifdef Q_OS_UNIX
	QFile randomFile("/dev/urandom");
	if (randomFile.open(QFile::ReadOnly)) {
		data = randomFile.read(20);
	}
#else
#	error "Secret::generateRandomSecret() isn't supported for your platform yet :(. Please file a bug report"
#endif

	if (data.length() >= 20) {
		rc = Secret(base32encode(data));
	}

	return rc;
}

std::string Secret::getShareHashStdString() const {
	QByteArray hash = getShareHash();
	return std::string(hash.constData(), hash.length());
}
