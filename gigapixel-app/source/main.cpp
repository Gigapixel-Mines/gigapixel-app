#include "fenetre.h"

#include <QApplication>
#include <QByteArray>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Fenetre* f = new Fenetre();
	f->show();
    return a.exec();
}
