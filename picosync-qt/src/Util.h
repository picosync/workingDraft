#ifndef UTIL_H
#define UTIL_H

#include <QByteArray>

class Util {
private:
	/// Disabled constructor
	Util() {}

public:
	/**
	 * \brief Small wrapper around cyoencode's base32 encoder that packs everything into Qt datatypes
	 * \param in Raw input data
	 * \returns Base32 encoded byte array
	 */
	static QByteArray base32encode(const QByteArray &in);

	/**
	 * \brief Small wrapper around cyodecode's base32 decoder that packs everything into Qt datatypes
	 * \param in Base32 input string
	 * \returns Raw data
	 */
	static QByteArray base32decode(const QByteArray &in);

	static QByteArray getRandomData(int len);
};

#endif // UTIL_H
