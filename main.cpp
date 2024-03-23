
#include "MainDialog.h"
#include "sock.h"
#include <QApplication>
#include <thread>

int main(int argc, char **argv)
{
	sock::startup();

	QApplication a(argc, argv);

	MainDialog w;
	w.exec();

	sock::cleanup();
	return 0;
}
