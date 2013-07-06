#ifndef CONNECTION_H
#define CONNECTION_H

#include <QHostAddress>
#include <QQueue>
#include <QUdpSocket>
#include <libutp/utp.h>

namespace Utp {
	class Connection: public QObject {
		Q_OBJECT
	private:
		QUdpSocket mSock;
		UTPSocket *mUtp;
		int mState;
		QQueue<QByteArray> mWriteBuffer;

		static void _sendData(void *userData, const byte *data, size_t len, const sockaddr *to, socklen_t toLen);

		static void _utpRead(void* userData, const byte* bytes, size_t count);
		static void _utpWrite(void* userData, byte* bytes, size_t count);
		static size_t _utpGetRbSize(void* userData);
		static void _utpState(void* userData, int state);
		static void _utpError(void* userData, int errcode);
		static void _utpOverhead(void *userData, bool send, size_t count, int type);

	public:
		Connection(quint16 localPort);

		void connect(const QHostAddress &host, const quint16 port);
		void send(const QByteArray &data);
	private slots:
		void _readPendingDatagrams();
	signals:
		void connected();
		void gotData(const QByteArray &utpData);
	};
}

#endif // CONNECTION_H
