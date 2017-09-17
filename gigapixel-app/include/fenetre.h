#ifndef FENETRE_H
#define FENETRE_H

#include "CapteurSpectral.h"
#include "FocusWindow.h"
#include "SerialCommunication.h"
#include "ZoneSelection.h"
#include "wcam.h"

#include <QCamera>
#include <QCameraViewfinder>
#include <QCameraImageCapture>
#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QMutex>
#include <QObject>
#include <QPushButton>
#include <QVideoWidget>
#include <QWidget>
#include <QtWidgets>

#define LONGUEUR_HORIZONTALE_CM 19.0 //A priori ok
#define LONGUEUR_VERTICALE_CM 13.0 //en réalité 17 cm mais course limitée à 13 cm
#define DISTANCE_HORIZONTALE_PAR_CRAN_CM 0.0000785
#define DISTANCE_VERTICALE_PAR_CRAN_CM 0.0000495

class Fenetre : public QWidget
{
	Q_OBJECT

private:
	QPushButton* autoFocus;
	QPushButton* avButton;
	QPushButton* arButton;
	QPushButton* goButton;
	QPushButton* stopButton;
	QPushButton* photoButton;
	QProgressBar* bar;
	QTabWidget* onglets;
	FocusWindow* focuswindow;
	QSpinBox* image_v;
	QSpinBox* image_h;
	QDoubleSpinBox* focusDistance;
	QDoubleSpinBox* sensorIntTime;
	QSlider* intTimeSlider;
	QLabel* label_v;
	QLabel* label_h;
	QLabel* label_spectre;
	QLabel* label_focus;
	QCheckBox* spectreActif;
	QGroupBox* createFirstExclusiveGroup();
	QGroupBox* createSecondExclusiveGroup();
	QWidget* miseAuPoint;
	QWidget* run;
	QWidget* cameraSpecs;
	QWidget* sensorSettings;
	QPushButton* zoneSelect;
	QGroupBox* polarizationGroupBox;
	QRadioButton* polarizationChoice[5];
	QPushButton* setSensorSettingsBtn;
	QComboBox* gain;
	QComboBox* upSensorList;
	QComboBox* downSensorList;

	//Onglet cam specs
	QSpinBox* camResHpx;
	QSpinBox* camResVpx;
	QDoubleSpinBox* pxHSize;
	QDoubleSpinBox* pxVSize;
	QPushButton* camSpecsValidateBtn;

	ZoneSelection* m_zoneSelection;

	CapteurSpectral* capteurHaut;
	CapteurSpectral* capteurBas;

	int nbPhotoH;
	int nbPhotoV;
	int nbrPhoto;

	int nbCranPasH;
	int nbCranPasV;

	int maxPhotoH;
	int maxPhotoV;

	int resHpx;
	int resVpx;

	int totalPixels;

	double sizeHpx;
	double sizeVpx;

	void Fenetre::closeEvent(QCloseEvent* event) override;
	void setCameraSpecs();
	void takeGigaPixelPhotoNoSpectrum();
	void updateProgressBar();

	bool m_saveSpectrumInfo;

	bool do_stop;
	bool taking_photo;
	bool sensorSet;

	QMutex stop_mutex;
	QMutex photo_mutex;
	QMutex buttonEnable_mutex;

	WCam* w;

public:
	Fenetre();
	~Fenetre();
	void Log(std::string strMsg);
	SerialCommunication* serialcomm;

private slots:
	void disableButton();
	void enableButton();
	void assign_h();
	void assign_v();
	//void compteur();
	void start();
	void stop();
	void setCamSpecsBtnPress();
	void openZoneSelect();
	void setCaptureBtn(bool);
	void setZonePic();
	void setIntTimeValue(int);
	void setSensorSettings();
	void refreshSensorsList();

signals:
	void LigneFinished();
	void PasGauche();
	void PasDroite();
	void FinCycle();
	void getSelZone();
};

#endif // FENETRE_H
