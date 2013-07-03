#ifndef MAIN_H
#define MAIN_H

#include "Discovery.h"

#include <QObject>
#include <QtCrypto/QtCrypto>

class Main: public QObject {
	Q_OBJECT
private:
	QCA::Initializer m_qcaInit;

	Discovery m_discovery;
public:
	explicit Main(QObject *parent = 0);
	~Main();

	void run();

private slots:
	void _gotPeerResponse(const Discovery::Announce &response);
	void _discoveryGotPacket(const QByteArray &pkg, const QHostAddress &sender, quint16 senderPort);

};

#endif // MAINWINDOW_H
