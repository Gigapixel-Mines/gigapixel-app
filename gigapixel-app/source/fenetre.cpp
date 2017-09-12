#include "fenetre.h"

#include <QApplication>
#include <QCameraInfo>

void Fenetre::Log(std::string strMsg)
{
    QString filename = "Data.txt";
    QFile file(filename);
    if (file.open(QIODevice::ReadWrite | QIODevice::Append))
    {
        QTextStream stream(&file);
        stream << QString::fromUtf8(strMsg.c_str()) << endl;
    }
}

void Fenetre::closeEvent(QCloseEvent * event)
{
	QApplication::quit();
}

Fenetre::Fenetre()
{
	Log("Démarrage du programme");
	// Fenetre utilisateur
	this->setAttribute(Qt::WA_DeleteOnClose);
	serialcomm = new SerialCommunication();
	onglets = new QTabWidget(this);
	onglets->setGeometry(10, 10, 400, 400);

	QWidget* miseAuPoint = new QWidget();
	QWidget* run = new QWidget();

	//Camera
	focuswindow = new FocusWindow();

	//Page Mise au point
	avButton = new QPushButton("Déplacement avant");
	arButton = new QPushButton("Déplacement arrière");
	QObject::connect(avButton, SIGNAL(pressed()), serialcomm, SLOT(miseAuPointAv()));
	QObject::connect(avButton, SIGNAL(released()), serialcomm, SLOT(miseAuPointStop()));
	QObject::connect(arButton, SIGNAL(pressed()), serialcomm, SLOT(miseAuPointAr()));
	QObject::connect(arButton, SIGNAL(released()), serialcomm, SLOT(miseAuPointStop()));
	//Log("Connection établie");

	QVBoxLayout *vboxMiseAuPoint = new QVBoxLayout;
	vboxMiseAuPoint->addWidget(avButton);
	vboxMiseAuPoint->addWidget(arButton);

	miseAuPoint->setLayout(vboxMiseAuPoint);

	//Page RUN
	label_v = new QLabel(tr("Nombre de photos verticales : "));
	image_v = new QSpinBox();
	image_v->setMinimum(1); //Pour test rapide
	image_v->setMaximum(32);
	image_v->setFixedSize(40, 20);
	pas_v = image_v->value();
	label_h = new QLabel(tr("Nombre de photos horizontales : "));
	image_h = new QSpinBox();
	image_h->setMinimum(1); //Pour test rapide
	image_h->setMaximum(29);
	pas_h = image_h->value();
	nbrPhoto = 1;
	goButton = new QPushButton("Marche");
	stopButton = new QPushButton("Arrêt");
	//  photoButton = new QPushButton ("Prendre photo");
	bar = new QProgressBar();

	/*  QVBoxLayout *vboxRun = new QVBoxLayout;
	  vboxRun->addWidget(label_h);
	  vboxRun->addWidget(image_h);
	  vboxRun->addWidget(label_v);
	  vboxRun->addWidget(image_v);
	  vboxRun->addWidget(photoButton);
	  vboxRun->addWidget(goButton);
	  vboxRun->addWidget(stopButton);
	  vboxRun->addWidget(bar);

	  run->setLayout(vboxRun); */

	QGridLayout *vgridRun = new QGridLayout();
	vgridRun->addWidget(createFirstExclusiveGroup(), 0, 0);
	vgridRun->addWidget(createSecondExclusiveGroup(), 1, 0);
	run->setLayout(vgridRun);

	//Réglages
	QObject::connect(image_h, SIGNAL(valueChanged(int)), this, SLOT(assign_h()));
	QObject::connect(image_v, SIGNAL(valueChanged(int)), this, SLOT(assign_v()));

	//photoButton def
	//QObject::connect(photoButton, SIGNAL(clicked()), focuswindow, SLOT(SaveImage()));
	//stopButton def

	//GoButton def
	QObject::connect(goButton, SIGNAL(clicked()), serialcomm, SLOT(initialPic()));
	QObject::connect(serialcomm, SIGNAL(InitFinished()), focuswindow, SLOT(SaveImage()));
	QObject::connect(focuswindow, SIGNAL(PictureTaken()), this, SLOT(compteur()));
	QObject::connect(this, SIGNAL(LigneFinished()), serialcomm, SLOT(haut()));
	QObject::connect(this, SIGNAL(PasDroite()), serialcomm, SLOT(droite()));
	QObject::connect(this, SIGNAL(PasGauche()), serialcomm, SLOT(gauche()));
	QObject::connect(this, SIGNAL(FinCycle()), this, SLOT(ableButton()));
	QObject::connect(serialcomm, SIGNAL(MvtFinished()), focuswindow, SLOT(SaveImage()));
	QObject::connect(goButton, SIGNAL(clicked()), this, SLOT(disableButton()));

	QObject::connect(stopButton, SIGNAL(clicked()), this, SLOT(stop()));
	QObject::connect(stopButton, SIGNAL(clicked()), this, SLOT(ableButton()));

	//Commentaire pour les onglets
	onglets->addTab(miseAuPoint, "Mise au point");
	onglets->addTab(run, "Prise de photos");

	setWindowTitle("gigapixel-app");
	//   setFixedSize(2000,2000);
	show();
	Log("Fenêtre principale lancée");
}

Fenetre::~Fenetre()
{
	Log("Fin de la session");
	Log("");
	delete serialcomm;
	delete focuswindow;
}

QGroupBox* Fenetre::createFirstExclusiveGroup()
{
	QGroupBox *groupBox = new QGroupBox(tr("Réglages"));
	QGridLayout * vgrid = new QGridLayout;
	vgrid->addWidget(label_v, 0, 0);
	vgrid->addWidget(image_v, 0, 1);
	vgrid->addWidget(label_h, 1, 0);
	vgrid->addWidget(image_h, 1, 1);
	groupBox->setLayout(vgrid);
	return groupBox;
}

QGroupBox* Fenetre::createSecondExclusiveGroup()
{
	QGroupBox *groupBox = new QGroupBox(tr("Mise en fonctionnement"));
	QVBoxLayout * vlayout = new QVBoxLayout;
	// vlayout->addWidget(photoButton);
	vlayout->addWidget(goButton);
	vlayout->addWidget(stopButton);
	vlayout->addWidget(bar);
	groupBox->setLayout(vlayout);
	return groupBox;
}

void Fenetre::compteur()
{
	Log("Compteur");
	alpha = alpha + 1;
	bar->setValue(bar->value() + 1);
	if (gamma == 1)
	{
		emit FinCycle();
		return;
	}
	if (alpha % pas_h == 0)
	{
		beta = beta + 1;
		if (beta < pas_v)
		{
			emit LigneFinished();
			return;
		}
		if (beta >= pas_v)
		{
			emit FinCycle();
			return;
		}
	}
	if (beta % 2 == 0)
	{
		emit PasDroite();
		return;
	}
	emit PasGauche();
	return;
}

void Fenetre::disableButton()
{
	//photoButton->setEnabled(false);
	avButton->setEnabled(false);
	arButton->setEnabled(false);
	goButton->setEnabled(false);
	image_h->setReadOnly(true);
	image_v->setReadOnly(true);
	alpha = 0;
	beta = 0;
	bar->setValue(0);
	nbrPhoto = pas_h * pas_v;
	bar->setMinimum(0);
	bar->setMaximum(nbrPhoto);
	gamma = 0;
}

void Fenetre::ableButton()
{
	//photoButton->setEnabled(true);
	avButton->setEnabled(true);
	arButton->setEnabled(true);
	goButton->setEnabled(true);
	image_h->setReadOnly(false);
	image_v->setReadOnly(false);
}

void Fenetre::assign_h()
{
	pas_h = image_h->value();
}

void Fenetre::assign_v()
{
	pas_v = image_v->value();
}

void Fenetre::stop()
{
	gamma = 1;
}
