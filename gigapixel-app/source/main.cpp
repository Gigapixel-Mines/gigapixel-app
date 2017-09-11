#include <QApplication>
#include <SerialCommunication.h>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#include <QStringList>
#include <QPushButton>
#include <QGridLayout>
#include <qdebug.h>
#include "fenetre.h"
#include "wcam.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Fenetre * f = new Fenetre();
	f->show();
	WCam * w = new WCam();
	//w->show();
    return a.exec();
}
