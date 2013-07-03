#include <QCoreApplication>
#include "Main.h"

#include "Discovery.h"

#include <stdio.h>

int main(int argc, char *argv[]) {
	QCoreApplication a(argc, argv);

	Main main;
	main.run();

	return a.exec();
}
