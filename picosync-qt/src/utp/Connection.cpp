#include "Connection.h"

Utp::Connection::Connection(quint16 localPort) {
	mSock.bind(localPort);
	mUtp = NULL;
	mState = UTP_STATE_CONNECT;

	qDebug("UTP connection listening at port %d", mSock.localPort());

	QObject::connect(&mSock, SIGNAL(readyRead()), this, SLOT(_readPendingDatagrams()));
}

void Utp::Connection::_readPendingDatagrams() {
	QHostAddress host;
	quint16 port;

	while (mSock.hasPendingDatagrams()) {
		qint64 size = mSock.pendingDatagramSize();
		char buff[size];
		size = mSock.readDatagram(buff, size, &host, &port);

		UTPGotIncomingConnection *_gotIncomingConnection = NULL; /// @todo implement incoming connection support

		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.s_addr = host.toIPv4Address();

		bool isUtp = UTP_IsIncomingUTP(_gotIncomingConnection, _sendData, this, reinterpret_cast<unsigned char*>(buff), size, (sockaddr*) &addr, sizeof(addr));
		if (!isUtp) {
			qWarning("Got packet that's not UTP (len %lld)", size);
		}
	}
}

void Utp::Connection::_sendData(void *userData, const byte *data, size_t len, const sockaddr *to, socklen_t /*toLen*/) {
	Connection *self = static_cast<Connection*>(userData);

	if (to->sa_family == AF_INET) {
		const sockaddr_in *addr_in = reinterpret_cast<const sockaddr_in*>(to);
		QHostAddress address(addr_in->sin_addr.s_addr);
		quint16 port = ntohs(addr_in->sin_port);

		qDebug("sendData (len: %ld, host: %s, port: %d)", len, qPrintable(address.toString()), port);

		self->mSock.writeDatagram(reinterpret_cast<const char*>(data), len, address, port);
	}
	else qWarning("UtpSocket::_sendData(): Unsupported address family: %i", to->sa_family);
}

void Utp::Connection::_utpError(void */*userData*/, int errcode) {
	qWarning("UTP error (code %d): %s", errcode, strerror(errcode));
}

size_t Utp::Connection::_utpGetRbSize(void */*userData*/) {
	qDebug("UtpGetRbSize");
	return 0;
}

void Utp::Connection::_utpOverhead(void */*userData*/, bool send, size_t count, int type) {
	qDebug("UtpOverhead: send=%s, count=%ld, type=%d", send?"true":"false", count, type);
}

void Utp::Connection::_utpRead(void *userData, const byte *bytes, size_t count) {
	Utp::Connection *self = static_cast<Utp::Connection*>(userData);
	QByteArray data(reinterpret_cast<const char*>(bytes), count);

	emit self->gotData(data);
}

void Utp::Connection::_utpState(void *userData, int state) {
	Utp::Connection *self = static_cast<Utp::Connection*>(userData);


	qDebug("UtpState: %d", state);

	switch(state) {
	case UTP_STATE_CONNECT:
		qDebug("UTP Socket connected.");
		emit self->connected();
		break;
	case UTP_STATE_WRITABLE:
		break;
	case UTP_STATE_EOF:
		break;
	case UTP_STATE_DESTROYING:
		break;
	}
}

void Utp::Connection::_utpWrite(void *userData, byte *bytes, size_t count) {
	Utp::Connection *self = static_cast<Utp::Connection*>(userData);

	qDebug("UtpWrite(count=%ld)", count);
	if (!self->mWriteBuffer.isEmpty()) {
		QByteArray data = self->mWriteBuffer.dequeue();
		/// @todo Check if data fits into the output buffer
		memcpy(bytes, data.constData(), qMin((size_t)data.length(), count));
	}
}

void Utp::Connection::connect(const QHostAddress &host, const quint16 port) {
	sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = host.toIPv4Address();
	addr.sin_port = htons(port);
	memset(addr.sin_zero, 0, sizeof(addr.sin_zero));

	mUtp = UTP_Create(reinterpret_cast<SendToProc*>(_sendData), this, (sockaddr*) &addr, sizeof(addr));
	UTP_SetSockopt(mUtp, SO_SNDBUF, 300*100);

	UTPFunctionTable utpCallbacks = {
		&_utpRead,
		&_utpWrite,
		&_utpGetRbSize,
		&_utpState,
		&_utpError,
		&_utpOverhead
	};
	UTP_SetCallbacks(mUtp, &utpCallbacks, this);
	UTP_Connect(mUtp);
}

void Utp::Connection::send(const QByteArray &data) {
	mWriteBuffer.enqueue(data);
	UTP_Write(mUtp, data.length());
}
