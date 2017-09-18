#include "fenetre.h"

#include <QApplication>
#include <QCameraInfo>
#include <QCheckBox>
#include <QFuture>
#include <QRadioButton>
#include <QSlider>
#include <QtConcurrent/QtConcurrent>
#include <Windows.h>

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
	camSpecsValidateBtn->setEnabled(false);
	run->setEnabled(false);
	miseAuPoint->setEnabled(false);
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
	absMaxPhotoH = floor(LONGUEUR_HORIZONTALE_CM / (tailleCapteurH - decalageHorizontal));
	absMaxPhotoV = floor(LONGUEUR_VERTICALE_CM / (tailleCapteurV - decalageVertical));

	m_zoneSelection->resetZone();

	serialcomm->setMaxPasH(absMaxPhotoH);
	serialcomm->setMaxPasH(absMaxPhotoV);
	image_v->setMaximum(absMaxPhotoV);
	image_h->setMaximum(absMaxPhotoH);
	serialcomm->setCransH(nbCranPasH);
	serialcomm->setCransV(nbCranPasV);
	serialcomm->envoieCranParPas();	//Appeler le port série pour envoyer le nombre de crans par pas
	camSpecsValidateBtn->setEnabled(true);
	run->setEnabled(true);
	miseAuPoint->setEnabled(true);
}

void Fenetre::setSensorSettings()
{
	if (upSensorList->currentIndex() == downSensorList->currentIndex()) //false for testing with one sensor
	{
		QMessageBox* error = new QMessageBox();
		error->setWindowTitle("Erreur");
		error->setText("Veuillez choisir deux capteurs différents");
		error->exec();
		return;
	}
	else
	{
		if (capteurBas != nullptr)
		{
			delete capteurBas;
			capteurBas = nullptr;
		}
		if (capteurHaut != nullptr)
		{
			delete capteurHaut;
			capteurHaut = nullptr;
		}
		//Comment one for test with one sensor
		capteurBas = new CapteurSpectral(downSensorList->currentText());
		capteurHaut = new CapteurSpectral(upSensorList->currentText());
		if (!capteurBas->setUp(intTimeSlider->value(), gain->currentIndex()))
		{
			Log("Erreur lors de l'envoi des paramètres au capteur bas");
			return;
		}
		if (!capteurHaut->setUp(intTimeSlider->value(), gain->currentIndex()))
		{
			Log("Erreur lors de l'envoi des paramètres au capteur bas");
			return;
		}
		sensorSet = true;
		return;
	}
}

void Fenetre::miseAuPointManuelleStart()
{
	serialcomm->miseAuPointManuelleStart();
	if (serialcomm->dataAvailable())
	{
		if (serialcomm->check('f'))
		{
			//Disable buttons
			zoneSelect->hide();
			sensorSettings->setEnabled(false);
			cameraSpecs->setEnabled(false);
			run->setEnabled(false);
			focusDistance->setEnabled(false);
			autoFocus->setEnabled(false);
			miseAuPointManuelleStartBtn->setEnabled(false);
			zoneSelect->setEnabled(false);
			QMessageBox* ok = new QMessageBox();
			ok->setWindowTitle("Réglage manuel");
			ok->setText("Vous pouvez utiliser le potentiomètre pour faire la mise au point");
			ok->exec();
			return;
		}
		else if (serialcomm->check('x'))
		{
			QMessageBox* info = new QMessageBox();
			info->setWindowTitle("Réglage manuel");
			info->setText("Veuillez mettre le potentiomètre sur 0 avant de lancer la mise au point manuelle");
			info->exec();
			return;
		}
	}
	else
	{
		Log("Erreur, impossible de communiquer avec l'Arduino");
		return;
	}
}

void Fenetre::miseAuPointManuelleStop()
{
	serialcomm->miseAuPointManuelleStop();
	if (serialcomm->dataAvailable())
	{
		if (serialcomm->check('g'))
		{
			//Enable what was disabled in start
			sensorSettings->setEnabled(true);
			cameraSpecs->setEnabled(true);
			run->setEnabled(true);
			focusDistance->setEnabled(true);
			autoFocus->setEnabled(true);
			miseAuPointManuelleStartBtn->setEnabled(true);
			zoneSelect->setEnabled(true);
			return;
		}
		else if (serialcomm->check('x'))
		{
			QMessageBox* info = new QMessageBox();
			info->setWindowTitle("Réglage manuel");
			info->setText("Veuillez mettre le potentiomètre sur 0 avant d'arrêter la mise au point manuelle");
			info->exec();
		}
	}
	else
	{
		Log("Erreur, impossible de communiquer avec l'Arduino");
		return;
	}
}

void Fenetre::refreshSensorsList()
{
	upSensorList->clear();
	downSensorList->clear();
	int portCount = QSerialPortInfo::availablePorts().count();
	if (portCount == 0)
	{
		Log("No serial port found");
		return;
	}
	else
	{
		QList<QSerialPortInfo> sPorts = QSerialPortInfo::availablePorts();
		foreach(const QSerialPortInfo& serialPortInfo, sPorts)
		{
			//QString portDesc = serialPortInfo.description();
			//qDebug() << serialPortInfo.hasProductIdentifier();
			//qDebug() << serialPortInfo.productIdentifier();
			//qDebug() << serialPortInfo.hasVendorIdentifier();
			//qDebug() << serialPortInfo.vendorIdentifier();
			//qDebug() << serialPortInfo.serialNumber();
			//qDebug() << serialPortInfo.manufacturer();
			//qDebug() << serialPortInfo.portName();
			if (serialPortInfo.hasProductIdentifier() == true &&
				serialPortInfo.hasVendorIdentifier() == true)
			{
				if (serialPortInfo.productIdentifier() == 24596 &&
					serialPortInfo.vendorIdentifier() == 1027)
				{
					upSensorList->addItem(serialPortInfo.serialNumber());
					downSensorList->addItem(serialPortInfo.serialNumber());
				}
			}
		}
		return;
	}
}

void Fenetre::takeGigaPixelPhoto()
{
	stop_mutex.lock();
	do_stop = false;
	stop_mutex.unlock();

	//Sleep(5000); //for threading synchronization test

	bool versLaDroite = true; //On commence à 0 et on ira vers la droite au début
	bool moving = false;

	for (int i = 1; i <= nbPhotoV; ++i)
	{
		for (int j = 1; j <= nbPhotoH; ++j) //on fait la ligne en premier
		{
			stop_mutex.lock(); //stop() peut accéder à do_stop aussi, on évite des soucis
			if (do_stop == false)
			{
				stop_mutex.unlock();
				//Les threads ne peuvent pas modifier la GUI, 
				//on envoie une demande pour avancer la barre de progrès
				QMetaObject::invokeMethod(this, "updateProgressBar");
				if (!focuswindow->SaveImage())
				{
					Log("Erreur lors de l'enregistrement de la photo");
					photo_mutex.lock();
					taking_photo = false;
					photo_mutex.unlock();
					QMetaObject::invokeMethod(this, "enableButton"); //Pareil que pour la barre
					//enableButton();
					//Enlever le polariseur
					return;
				}
				if (versLaDroite)
				{
					moving = true;
					if (!serialcomm->droite())
					{
						Log("Erreur lors du déplacement vers la droite");
						photo_mutex.lock();
						taking_photo = false;
						photo_mutex.unlock();
						QMetaObject::invokeMethod(this, "enableButton"); //Pareil que pour la barre
						return;
					}
				}
				else
				{
					moving = true;
					if (!serialcomm->gauche())
					{
						Log("Erreur lors du déplacement vers la gauche");
						photo_mutex.lock();
						taking_photo = false;
						photo_mutex.unlock();
						QMetaObject::invokeMethod(this, "enableButton"); //Pareil que pour la barre
						return;
					}
				}
				//check what comes
				do
				{
					if (serialcomm->dataAvailable())
					{
						switch (serialcomm->getChar())
						{
						case 'x': //Signal to get 
							if (m_saveSpectrumInfo)
							{
								QMetaObject::invokeMethod(this, "getSpecData");
							}
							break;
						case 'z':
							moving = false;
							break;
						default:
							Log("Invalid answer");
							photo_mutex.lock();
							taking_photo = false;
							photo_mutex.unlock();
							QMetaObject::invokeMethod(this, "enableButton");
							return;
							break;
						}
					}
					else
					{
						Log("Serial Port timed out");
						photo_mutex.lock();
						taking_photo = false;
						photo_mutex.unlock();
						QMetaObject::invokeMethod(this, "enableButton");
						return;
						break;
					}
				} while (moving);
			}
			else
			{
				stop_mutex.unlock();
				Log("Arrêt");
				photo_mutex.lock();
				taking_photo = false;
				photo_mutex.unlock();
				QMetaObject::invokeMethod(this, "enableButton");
				//enableButton();
				//Enlever le polariseur
				return; //On sort de la fonction ici
			}
		}
		moving = true;
		if (!serialcomm->haut())
		{
			Log("Erreur lors du déplacement vers le haut");
			photo_mutex.lock();
			taking_photo = false;
			photo_mutex.unlock();
			QMetaObject::invokeMethod(this, "enableButton"); //Pareil que pour la barre
			return;
		}
		do
		{
			if (serialcomm->dataAvailable())
			{
				switch (serialcomm->getChar())
				{
				case 'x': //Signal to get 
					if (m_saveSpectrumInfo)
					{
						QMetaObject::invokeMethod(this, "getSpecData");
					}
					break;
				case 'z':
					moving = false;
					break;
				default:
					Log("Invalid answer");
					photo_mutex.lock();
					taking_photo = false;
					photo_mutex.unlock();
					QMetaObject::invokeMethod(this, "enableButton");
					return;
					break;
				}
			}
			else
			{
				Log("Serial Port timed out");
				photo_mutex.lock();
				taking_photo = false;
				photo_mutex.unlock();
				QMetaObject::invokeMethod(this, "enableButton");
				return;
				break;
			}
		} while (moving);
		versLaDroite = !versLaDroite; //On change de direction après avoir fait une ligne
	}
	Log("Fin de la prise de photo");
	photo_mutex.lock();
	taking_photo = false;
	photo_mutex.unlock();
	QMetaObject::invokeMethod(this, "enableButton");
	//enableButton();
	//Enlever le polariseur
	return;
}

void Fenetre::updateProgressBar()
{
	bar->setValue(bar->value() + 1);
}

Fenetre::Fenetre()
	: do_stop(false)
	, stop_mutex()
	, photo_mutex()
	, buttonEnable_mutex()
	, m_saveSpectrumInfo(false)
	, taking_photo(false)
	, sensorSet(false)
	, capteurHaut(nullptr)
	, capteurBas(nullptr)
{
	Log("Démarrage du programme");
	// Fenetre utilisateur
	this->setAttribute(Qt::WA_DeleteOnClose);
	serialcomm = new SerialCommunication();
	onglets = new QTabWidget(this);
	onglets->setGeometry(10, 10, 620, 460);
	this->setFixedSize(640, 480);

	miseAuPoint = new QWidget();
	run = new QWidget();
	cameraSpecs = new QWidget();

	//Camera
	focuswindow = new FocusWindow();

	//Sélection de la zone de la photo à enregistrer
	m_zoneSelection = new ZoneSelection();
	QObject::connect(m_zoneSelection, SIGNAL(sendCoordsToWindow(QPoint, QPoint)), this, SLOT(setStartingCoordAndBounds(QPoint, QPoint)));
	QObject::connect(m_zoneSelection, SIGNAL(resetCoordsToWindow()), this, SLOT(resetStartingCoordsAndBounds()));
	//m_zoneSelection->show();

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

	//Page réglage capteur AMS
	QGridLayout* vgridSensorSettings = new QGridLayout();
	sensorSettings = new QWidget();
	QPushButton* refreshSensorsListBtn = new QPushButton("Rafraichir la liste des capteurs");
	QObject::connect(refreshSensorsListBtn, SIGNAL(clicked()), this, SLOT(refreshSensorsList()));
	QLabel* upSensorLabel = new QLabel("Capteur spectral supérieur : ");
	QLabel* downSensorLabel = new QLabel("Capteur spectral inférieur : ");
	upSensorList = new QComboBox();
	upSensorList->setEditable(false);
	downSensorList = new QComboBox();
	downSensorList->setEditable(false);
	sensorIntTime = new QDoubleSpinBox();
	intTimeSlider = new QSlider(Qt::Horizontal);
	sensorIntTime->setSingleStep(2.8);
	sensorIntTime->setRange(1 * 2.8, 255 * 2.8); //cf datasheet
	sensorIntTime->setReadOnly(true);
	sensorIntTime->setSuffix(" ms");
	sensorIntTime->setFixedWidth(100);
	sensorIntTime->setValue(128 * 2.8);
	intTimeSlider->setRange(1, 255);
	intTimeSlider->setSingleStep(1);
	intTimeSlider->setValue(128);
	intTimeSlider->setFixedWidth(200);
	QObject::connect(intTimeSlider, SIGNAL(valueChanged(int)), this, SLOT(setIntTimeValue(int)));
	QLabel* intTimeLabel = new QLabel("Temps d'intégration :");
	setSensorSettingsBtn = new QPushButton("Valider les paramètres des capteurs");
	QObject::connect(setSensorSettingsBtn, SIGNAL(clicked()), this, SLOT(setSensorSettings()));
	QLabel* gainLabel = new QLabel("Gain");
	gain = new QComboBox();
	gain->addItem("x1");
	gain->addItem("x3.7");
	gain->addItem("x16");
	gain->addItem("x64");
	gain->setEditable(false);
	vgridSensorSettings->addWidget(refreshSensorsListBtn, 0, 0, Qt::AlignTop);
	vgridSensorSettings->addWidget(upSensorLabel, 1, 0, Qt::AlignTop);
	vgridSensorSettings->addWidget(upSensorList, 1, 1, Qt::AlignTop);
	vgridSensorSettings->addWidget(downSensorLabel, 2, 0, Qt::AlignTop);
	vgridSensorSettings->addWidget(downSensorList, 2, 1, Qt::AlignTop);
	vgridSensorSettings->addWidget(intTimeLabel, 3, 0, Qt::AlignTop);
	vgridSensorSettings->addWidget(intTimeSlider, 3, 1, Qt::AlignTop);
	vgridSensorSettings->addWidget(sensorIntTime, 3, 2, Qt::AlignTop);
	vgridSensorSettings->addWidget(gainLabel, 4, 0, Qt::AlignTop);
	vgridSensorSettings->addWidget(gain, 4, 1, Qt::AlignTop);
	vgridSensorSettings->addWidget(setSensorSettingsBtn, 5, 0, Qt::AlignTop);
	sensorSettings->setLayout(vgridSensorSettings);

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

	QObject::connect(autoFocus, SIGNAL(clicked()), serialcomm, SLOT(miseAuPointAuto()));

	QGroupBox* manFocusGroup = new QGroupBox(tr("Mise au point manuelle"));
	QGridLayout* vgridManFocus = new QGridLayout();
	miseAuPointManuelleStartBtn = new QPushButton("Commencer mise au point manuelle");
	miseAuPointManuelleStopBtn = new QPushButton("Arrêter mise au point manuelle");
	zoneSelect = new QPushButton("Sélection de la zone à photographier"); //test
	vgridManFocus->addWidget(miseAuPointManuelleStartBtn, 0, 0);
	vgridManFocus->addWidget(miseAuPointManuelleStopBtn, 1, 0);
	vgridManFocus->addWidget(zoneSelect, 2, 0); //Test
	manFocusGroup->setLayout(vgridManFocus);
	QObject::connect(miseAuPointManuelleStartBtn, SIGNAL(clicked()), this, SLOT(miseAuPointManuelleStart()));
	QObject::connect(miseAuPointManuelleStopBtn, SIGNAL(clicked()), this, SLOT(miseAuPointManuelleStop()));
	QObject::connect(zoneSelect, SIGNAL(clicked()), this, SLOT(openZoneSelect())); //Test

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
	nbPhotoV = image_v->value();
	label_h = new QLabel(tr("Nombre de photos horizontales : "));
	image_h = new QSpinBox();
	image_h->setMinimum(1); //Pour test rapide
	image_h->setMaximum(33);
	image_h->setFixedSize(40, 20);
	nbPhotoH = image_h->value();
	nbrPhoto = 1;
	goButton = new QPushButton("Marche");
	stopButton = new QPushButton("Arrêt");
	//  photoButton = new QPushButton ("Prendre photo");
	bar = new QProgressBar();

	polarizationGroupBox = new QGroupBox(tr("Polarisation"));
	QGridLayout* tmpVgrid = new QGridLayout;
	polarizationChoice[0] = new QRadioButton("Sans polariseur");
	polarizationChoice[1] = new QRadioButton("Polariseur à 0° (vertical)");
	polarizationChoice[2] = new QRadioButton("Polariseur à 45° (sens horaire)");
	polarizationChoice[3] = new QRadioButton("Polariseur à 90° (horizontal)");
	polarizationChoice[4] = new QRadioButton("Polariseur à 135° (sens horaire)");
	polarizationChoice[0]->setChecked(true);
	for (int i = 0; i < 5; ++i)
	{
		tmpVgrid->addWidget(polarizationChoice[i]);
	}
	polarizationGroupBox->setLayout(tmpVgrid);

	QGridLayout* vgridRun = new QGridLayout();
	vgridRun->addWidget(createFirstExclusiveGroup(), 0, 0);
	vgridRun->addWidget(polarizationGroupBox, 1, 0);
	vgridRun->addWidget(createSecondExclusiveGroup(), 2, 0);
	run->setLayout(vgridRun);

	//Réglages
	QObject::connect(image_h, SIGNAL(valueChanged(int)), this, SLOT(assign_h()));
	QObject::connect(image_v, SIGNAL(valueChanged(int)), this, SLOT(assign_v()));

	//photoButton def
	//QObject::connect(photoButton, SIGNAL(clicked()), focuswindow, SLOT(SaveImage()));
	//stopButton def

	//GoButton def
	//QObject::connect(goButton, SIGNAL(clicked()), serialcomm, SLOT(initialPic()));
	//QObject::connect(serialcomm, SIGNAL(InitFinished()), focuswindow, SLOT(SaveImage()));
	//QObject::connect(focuswindow, SIGNAL(PictureTaken()), this, SLOT(compteur()));
	//QObject::connect(this, SIGNAL(LigneFinished()), serialcomm, SLOT(haut()));
	//QObject::connect(this, SIGNAL(PasDroite()), serialcomm, SLOT(droite()));
	//QObject::connect(this, SIGNAL(PasGauche()), serialcomm, SLOT(gauche()));
	//QObject::connect(this, SIGNAL(FinCycle()), this, SLOT(ableButton()));
	//QObject::connect(serialcomm, SIGNAL(MvtFinished()), focuswindow, SLOT(SaveImage()));

	QObject::connect(goButton, SIGNAL(clicked()), this, SLOT(start()));

	QObject::connect(stopButton, SIGNAL(clicked()), this, SLOT(stop()));
	//QObject::connect(stopButton, SIGNAL(clicked()), this, SLOT(enableButton()));

	//Commentaire pour les onglets
	onglets->addTab(cameraSpecs, "Propriétés de la caméra");
	onglets->addTab(sensorSettings, "Paramètres des capteurs spectraux");
	onglets->addTab(miseAuPoint, "Mise au point");
	onglets->addTab(run, "Prise de photos");

	//Par défaut on set les paramètres de la MAKO G-503
	setCameraSpecs();

	setWindowTitle("gigapixel-app");
	//   setFixedSize(2000,2000);
	show();
	w = new WCam();
	QObject::connect(this, SIGNAL(getSelZone()), w, SLOT(getImage()));
	QObject::connect(w, SIGNAL(canCapture(bool)), this, SLOT(setCaptureBtn(bool)));
	QObject::connect(w, SIGNAL(selZonePicReady()), this, SLOT(setZonePic()));

	Log("Fenêtre principale lancée");
}

Fenetre::~Fenetre()
{
	Log("Fin de la session");
	Log("");
	delete serialcomm;
	delete focuswindow;
	delete capteurBas;
	delete capteurHaut;
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

//void Fenetre::compteur()
//{
//	Log("Compteur");
//	alpha = alpha + 1;
//	bar->setValue(bar->value() + 1);
//	if (gamma == 1)
//	{
//		emit FinCycle();
//		return;
//	}
//	if (alpha % pas_h == 0)
//	{
//		beta = beta + 1;
//		if (beta < pas_v)
//		{
//			emit LigneFinished();
//			return;
//		}
//		if (beta >= pas_v)
//		{
//			emit FinCycle();
//			return;
//		}
//	}
//	if (beta % 2 == 0)
//	{
//		emit PasDroite();
//		return;
//	}
//	emit PasGauche();
//	return;
//}

void Fenetre::start()
{
	photo_mutex.lock();
	if (taking_photo)
	{
		photo_mutex.unlock();
		Log("Already taking photo");
		return;
	}
	photo_mutex.unlock();

	disableButton();
	focuswindow->resetNbPhoto();

	//test
	//takeGigaPixelPhotoNoSpectrum();
	//QFuture<void> thread1 = QtConcurrent::run(this, &Fenetre::takeGigaPixelPhotoNoSpectrum);

	int polarization(0);
	for (int i = 0; i < 5; ++i)
	{
		if (polarizationChoice[i]->isChecked())
		{
			polarization = i;
			dataDir = focuswindow->setPolarization(polarization);
			saveSpecData("V,B,G,Y,O,R", "haut");
			saveSpecData("V,B,G,Y,O,R", "bas");
			break;
		}
	}

	//Code pour mettre en place le polariseur

	//Récupération de la référence 0,0 en X, Y
	if (!serialcomm->findXYRef())
	{
		Log("Erreur lors de la recherche de la référence XY");
		return;
	}
	else
	{
		m_saveSpectrumInfo = spectreActif->isChecked();
		if (m_saveSpectrumInfo)
		{
			if (!sensorSet)
			{
				QMessageBox* error = new QMessageBox();
				error->setWindowTitle("Erreur");
				error->setText("Les capteurs spectraux ne sont pas paramétrés");
				error->exec();
				enableButton();
				return;
			}
		}
		photo_mutex.lock();
		taking_photo = true;
		photo_mutex.unlock();
		focuswindow->stopImgRefresh(true); //Avoid threading problem with sync in focuswindow
		QFuture<void> thread1 = QtConcurrent::run(this, &Fenetre::takeGigaPixelPhoto);
	}
	return;
}

void Fenetre::disableButton()
{
	//photoButton->setEnabled(false);
	m_zoneSelection->hide();
	buttonEnable_mutex.lock();
	miseAuPointManuelleStartBtn->setEnabled(false);
	miseAuPointManuelleStopBtn->setEnabled(false);
	goButton->setEnabled(false);
	spectreActif->setEnabled(false);
	image_h->setEnabled(false);
	image_v->setEnabled(false);
	//alpha = 0;
	//beta = 0;
	bar->setValue(0);
	nbrPhoto = nbPhotoH * nbPhotoV;
	bar->setMinimum(0);
	bar->setMaximum(nbrPhoto);
	miseAuPoint->setEnabled(false);
	sensorSettings->setEnabled(false);
	cameraSpecs->setEnabled(false);
	polarizationGroupBox->setEnabled(false);
	//gamma = 0;
	buttonEnable_mutex.unlock();
}

void Fenetre::enableButton()
{
	//photoButton->setEnabled(true);
	buttonEnable_mutex.lock();
	miseAuPointManuelleStartBtn->setEnabled(true);
	miseAuPointManuelleStopBtn->setEnabled(true);
	goButton->setEnabled(true);
	spectreActif->setEnabled(true);
	image_h->setEnabled(true);
	image_v->setEnabled(true);
	miseAuPoint->setEnabled(true);
	sensorSettings->setEnabled(true);
	cameraSpecs->setEnabled(true);
	polarizationGroupBox->setEnabled(true);
	focuswindow->stopImgRefresh(false);
	buttonEnable_mutex.unlock();
}

void Fenetre::assign_h()
{
	nbPhotoH = image_h->value();
}

void Fenetre::assign_v()
{
	nbPhotoV = image_v->value();
}

void Fenetre::stop()
{
	stop_mutex.lock();
	//gamma = 1;
	do_stop = true;
	stop_mutex.unlock();
}

void Fenetre::setCamSpecsBtnPress()
{
	return setCameraSpecs();
}

void Fenetre::openZoneSelect()
{
	emit getSelZone();
}

void Fenetre::setZonePic()
{
	QPixmap pixmap;
	QImage temp = w->getZonePic();
	pixmap = QPixmap::fromImage(temp);
	qDebug() << pixmap.isNull();
	m_zoneSelection->setImageForSelection(pixmap);
	m_zoneSelection->show();
}

void Fenetre::setIntTimeValue(int value)
{
	sensorIntTime->setValue(value * 2.8);
}

void Fenetre::setCaptureBtn(bool ready)
{
	zoneSelect->setEnabled(ready);
}

void Fenetre::getSpecData()
{
	QString hautData;
	QString basData;
	hautData = capteurHaut->getData();
	basData = capteurBas->getData();
	hautData.chop(3);
	basData.chop(3);
	hautData = hautData.simplified();
	hautData.replace(" ", "");
	basData = hautData.simplified();
	basData.replace(" ", "");
	saveSpecData(hautData, "haut");
	saveSpecData(basData, "bas");
}

void Fenetre::saveSpecData(QString t_data, QString t_sensor)
{
	QString filename = dataDir + "capteur_" + t_sensor + "_data.txt";
	QFile file(filename);
	if (file.open(QIODevice::ReadWrite | QIODevice::Append))
	{
		QTextStream stream(&file);
		stream << t_data << endl;
	}
}

void Fenetre::resetStartingCoordsAndBounds()
{
	startCoordPasH = 0;
	startCoordPasV = 0;
	startCoordCransH = 0;
	startCoordCransV = 0;
	image_h->setMaximum(absMaxPhotoH);
	image_v->setMaximum(absMaxPhotoV);
	//TODO : recalculer max pasH max pasV max cranH et max cranV
}

void Fenetre::setStartingCoordAndBounds(QPoint botLeft, QPoint topRight)
{
	//TODO : convertir les coordonnées du QRect en pas et crans compréhensible par l'Arduino
	//Modifier les starts coords

	startCoordPasH = 0; //Adapter les valeurs
	startCoordPasV = 0;
	startCoordCransH = 0;
	startCoordCransV = 0;
	//Modifier les limites pour la zone sélectionnée
	maxPhotoH = floor(0);
	maxPhotoV = floor(0);
	image_h->setMaximum(maxPhotoH);
	image_v->setMaximum(maxPhotoV);
}
