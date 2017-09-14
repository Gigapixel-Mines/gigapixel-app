#include "fenetre.h"

#include <QApplication>
#include <QCameraInfo>
#include <QCheckBox>

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

void Fenetre::setCameraSpecs()
{
	resHpx = camResHpx->value();
	resVpx = camResVpx->value();
	totalPixels = resHpx * resVpx;
	sizeHpx = pxHSize->value() / 10000; // en cm
	sizeVpx = pxVSize->value() / 10000; // en cm
	double tailleCapteurH = resHpx * sizeHpx;
	double tailleCapteurV = resVpx * sizeVpx;
	double recouvre_5_pourcent = floor(0.05 * totalPixels);
	double decalageHorizontal = recouvre_5_pourcent / resVpx; //en pixel
	double decalageVertical = recouvre_5_pourcent / resHpx;
	decalageHorizontal = decalageHorizontal * sizeHpx; //en cm
	decalageVertical = decalageVertical * sizeVpx; //en cm
	nbCranPasH = static_cast<int>((tailleCapteurH - decalageHorizontal) / DISTANCE_HORIZONTALE_PAR_CRAN_CM);
	nbCranPasV = static_cast<int>((tailleCapteurV - decalageVertical) / DISTANCE_VERTICALE_PAR_CRAN_CM);
	maxPhotoH = floor(LONGUEUR_HORIZONTALE_CM / (tailleCapteurH - decalageHorizontal));
	maxPhotoV = floor(LONGUEUR_VERTICALE_CM / (tailleCapteurV - decalageVertical));
	image_v->setMaximum(maxPhotoV);
	image_h->setMaximum(maxPhotoH);
	serialcomm->setCransH(nbCranPasH);
	serialcomm->setCransV(nbCranPasV);
	//Appeler le port série pour envoyer le nombre de crans par pas
}

void Fenetre::takeGigaPixelPhoto()
{
}

Fenetre::Fenetre() : do_stop(false)
{
	Log("Démarrage du programme");
	// Fenetre utilisateur
	this->setAttribute(Qt::WA_DeleteOnClose);
	serialcomm = new SerialCommunication();
	onglets = new QTabWidget(this);
	onglets->setGeometry(10, 10, 400, 400);

	QWidget* miseAuPoint = new QWidget();
	QWidget* run = new QWidget();
	QWidget* cameraSpecs = new QWidget();

	//Camera
	focuswindow = new FocusWindow();

	//Page caméra specs
	QGridLayout* vgridCamSpecs = new QGridLayout();
	QLabel* camResHpx_label = new QLabel("Résolution horizontale de la caméra en pixels :");
	QLabel* camResVpx_label = new QLabel("Résolution verticale de la caméra en pixels :");
	camResHpx = new QSpinBox();
	camResHpx->setMinimum(0);
	camResHpx->setMaximum(1000000);
	camResHpx->setValue(2592);
	camResHpx->setSingleStep(1);
	camResHpx->setFixedSize(100, 20);
	camResVpx = new QSpinBox();
	camResVpx->setMinimum(0);
	camResVpx->setMaximum(1000000);
	camResVpx->setValue(1944);
	camResVpx->setSingleStep(1);
	camResVpx->setFixedSize(100, 20);
	QLabel* pxHSize_label = new QLabel("Taille horizontale du pixel en µm :");
	QLabel* pxVSize_label = new QLabel("Taille verticale du pixel en µm :");
	pxHSize = new QDoubleSpinBox();
	pxHSize->setMinimum(0.1);
	pxHSize->setValue(2.2);
	pxHSize->setSingleStep(0.1);
	pxHSize->setFixedSize(60, 20);
	pxVSize = new QDoubleSpinBox();
	pxVSize->setMinimum(0.1);
	pxVSize->setValue(2.2);
	pxVSize->setSingleStep(0.1);
	pxVSize->setFixedSize(60, 20);
	camSpecsValidateBtn = new QPushButton("Valider les paramètres de la caméra");
	vgridCamSpecs->addWidget(camResHpx_label, 0, 0);
	vgridCamSpecs->addWidget(camResHpx, 0, 1);
	vgridCamSpecs->addWidget(camResVpx_label, 1, 0);
	vgridCamSpecs->addWidget(camResVpx, 1, 1);
	vgridCamSpecs->addWidget(pxHSize_label, 2, 0);
	vgridCamSpecs->addWidget(pxHSize, 2, 1);
	vgridCamSpecs->addWidget(pxVSize_label, 3, 0);
	vgridCamSpecs->addWidget(pxVSize, 3, 1);
	vgridCamSpecs->addWidget(camSpecsValidateBtn);
	cameraSpecs->setLayout(vgridCamSpecs);
	QObject::connect(camSpecsValidateBtn, SIGNAL(clicked()), this, SLOT(setCamSpecsBtnPress()));


	//Page Mise au point
	QGroupBox* autoFocusGroup = new QGroupBox(tr("Mise au point semi-auto"));
	QGridLayout* vgridAutoFocus = new QGridLayout();
	focusDistance = new QDoubleSpinBox();
	focusDistance->setMinimum(0.0);
	focusDistance->setValue(1.2);
	focusDistance->setSingleStep(0.1);
	focusDistance->setFixedSize(60, 20);
	label_focus = new QLabel("Distance de mise au point en m : ");
	autoFocus = new QPushButton("Mise au point");
	vgridAutoFocus->addWidget(label_focus, 0, 0);
	vgridAutoFocus->addWidget(focusDistance, 0, 1);
	vgridAutoFocus->addWidget(autoFocus, 1, 1);
	autoFocusGroup->setLayout(vgridAutoFocus);

	QGroupBox* manFocusGroup = new QGroupBox(tr("Mise au point manuelle"));
	QGridLayout* vgridManFocus = new QGridLayout();
	avButton = new QPushButton("Déplacement avant");
	arButton = new QPushButton("Déplacement arrière");
	vgridManFocus->addWidget(avButton, 0, 0);
	vgridManFocus->addWidget(arButton, 1, 0);
	manFocusGroup->setLayout(vgridManFocus);
	QObject::connect(avButton, SIGNAL(pressed()), serialcomm, SLOT(miseAuPointAv()));
	QObject::connect(avButton, SIGNAL(released()), serialcomm, SLOT(miseAuPointStop()));
	QObject::connect(arButton, SIGNAL(pressed()), serialcomm, SLOT(miseAuPointAr()));
	QObject::connect(arButton, SIGNAL(released()), serialcomm, SLOT(miseAuPointStop()));

	QGridLayout* vgridFocus = new QGridLayout();
	vgridFocus->addWidget(autoFocusGroup, 0, 0);
	vgridFocus->addWidget(manFocusGroup, 1, 0);
	miseAuPoint->setLayout(vgridFocus);


	//Page RUN
	spectreActif = new QCheckBox();
	label_spectre = new QLabel(tr("Prendre les informations spectrales : "));
	label_v = new QLabel(tr("Nombre de photos verticales : "));
	image_v = new QSpinBox();
	image_v->setMinimum(1); //Pour test rapide
	image_v->setMaximum(33);
	image_v->setFixedSize(40, 20);
	pas_v = image_v->value();
	label_h = new QLabel(tr("Nombre de photos horizontales : "));
	image_h = new QSpinBox();
	image_h->setMinimum(1); //Pour test rapide
	image_h->setMaximum(33);
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

	QGridLayout* vgridRun = new QGridLayout();
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
	onglets->addTab(cameraSpecs, "Propriétés de la caméra");
	onglets->addTab(miseAuPoint, "Mise au point");
	onglets->addTab(run, "Prise de photos");

	//Par défaut on set les paramètres de la MAKO G-503
	setCameraSpecs();

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
	vgrid->addWidget(label_spectre, 2, 0);
	vgrid->addWidget(spectreActif, 2, 1);
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
	spectreActif->setEnabled(false);
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
	spectreActif->setEnabled(true);
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

void Fenetre::setCamSpecsBtnPress()
{
	return setCameraSpecs();
}
