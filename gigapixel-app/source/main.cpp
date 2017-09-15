#include "fenetre.h"
#include "wcam.h"

#include <QApplication>
#include <QByteArray>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Fenetre* f = new Fenetre();
	f->show();
	WCam* w = new WCam();
	//w->show();
    return a.exec();
}
