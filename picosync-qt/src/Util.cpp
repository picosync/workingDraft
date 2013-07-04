#include "Util.h"

#include <QFile>
#include <cyoencode-1.0.2/src/CyoDecode.h>
#include <cyoencode-1.0.2/src/CyoEncode.h>

QByteArray Util::base32decode(const QByteArray &in) {
	QByteArray rc;
	rc.reserve(cyoBase32DecodeGetLength(in.length()));

	rc.resize(cyoBase32Decode(rc.data(), in.constData(), in.length()));

	return rc;
}

QByteArray Util::base32encode(const QByteArray &in) {
	QByteArray rc;
	rc.reserve(cyoBase32EncodeGetLength(in.length()));
	rc.resize(cyoBase32Encode(rc.data(), in.constData(), in.length()));

	return rc;
}

QByteArray Util::getRandomData(int len) {
	QByteArray rc;

#ifdef Q_OS_UNIX
	QFile randomFile("/dev/urandom");
	if (randomFile.open(QFile::ReadOnly)) {
		rc = randomFile.read(len);
	}
#else
#	error "Random::getRandomData() isn't supported for your platform yet. Please file a bug report"
#endif

	return rc;
}
