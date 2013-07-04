#ifndef SECRET_H
#define SECRET_H

#include <QByteArray>
#include <string>


/**
 * \brief Represents a Share secret.
 *
 * Some characteristics of share secrets:
 * \li The R/O secret is simply a hashed version of the normal secret
 * \li The share hash is calculated by
 * \li R/O secrets start with the letter R, normal secrets don't
 * \immutable
 */
class Secret {
public:
	enum Type {
		/// Normal secret, hashing it leads to the R/O secret
		SecretType,
		/// Read only secret, hashing it leads to the share hash (the one that's transmitted via the wire)
		RoSecretType
	};

private:
	/// Normal secret in base32 form (may be null)
	QByteArray mSecret;

	/// Read only secret in base32 form
	QByteArray mRoSecret;

	/// Share hash in binary form
	QByteArray mShareHash;

	/// Secret type (normal/read only)
	Type mType;

	/**
	 * \brief Initialization function, calculates and sets mSecret, mRoSecret and mShareHash
	 * \param secret Normal or read only secret, base32 encoded
	 */
	void _init(const QByteArray &secret);
public:
	/**
	 * \brief Constructor, takes a normal or a r/o secret
	 * \param secret Normal or read only secret in base32 form
	 * \note R/O secrets always start with an R, normal ones don't, so the
	 *	parameter's type is determined automatically here
	 * \todo Check if secret's base32 encoded
	 */
	Secret(const QByteArray &secret);

	/**
	 * \brief Convenience constructor, wraps the c string into a byte array and calls _init()
	 * \param secret Normal or read only secret, base32 encoded
	 */
	Secret(const char *secret);

	/**
	 * \brief Default constructor, creates an invalid Secret
	 */
	Secret();

	/**
	 * \brief Returns the binary version of the readonly secret (removed the
	 * leading 'R' character and base32-decoded)
	 * \returns Binary secret
	 */
	QByteArray getBinaryRoSecret() const;

	/**
	 * \brief Returns the normal secret in base32 form
	 * \returns Secret (only works if getType() is SecretType)
	 * \note Returns an empty byte array if it fails
	 */
	QByteArray getSecret() const {
		return mSecret;
	}

	/**
	 * \brief Returns the read only secret in base32 form
	 * \returns Read only secret (including the 'R' prefix)
	 */
	QByteArray getRoSecret() const {
		return mRoSecret;
	}

	/**
	 * \brief Returns the share hash of this secret (in binary form)
	 * \returns The share's hash (the one that'll be sent over the wire)
	 */
	QByteArray getShareHash() const {
		return mShareHash;
	}

	std::string getShareHashStdString() const;

	bool isValid() const {
		return !mShareHash.isNull();
	}

	/**
	 * \brief Returns the type of this secret
	 * \returns Secret type
	 */
	void getType();

	/**
	 * \brief Generates and returns a random secret using /dev/urandom
	 * \returns New secret (or an invalid secret if there was an error)
	 */
	static Secret generateRandomSecret();

	/**
	 * \brief Creates a Secret from a Share hash
	 *
	 * The new Secret will return null QByteArrays when calling getRoSecret() or getSecret()
	 * and therefore can not be used as encryption key
	 * \param shareHash Share hash (length: 32bytes if binary, 64 chars if hex encoded)
	 * \returns New Secret
	 */
	static Secret fromShareHash(const QByteArray &shareHash);

	static Secret fromShareHash(const std::string &shareHash) {
		return fromShareHash(QByteArray(shareHash.data(), shareHash.length()));
	}
};

#endif // SECRET_H
