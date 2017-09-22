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
//Calibration distance/pixels pour la webcam
//#define PX_H_WCAM_TO_CM 0
//#define PX_V_WCAM_TO_CM 0
//Distance entre le capteur spectral haut selon l'horizontale au coin en bas à gauche du CCD
#define OFFSET_CAPTEUR_HAUT_HORI_CM -2.4 //Valeur à calibrer, peut être négatif si à gauche
//Distance entre le capteur spectral haut selon la verticale au coin en bas à gauche du CCD
#define OFFSET_CAPTEUR_HAUT_VERTI_CM 1.3
//Distance entre le capteur spectral bas selon l'horizontale au coin en bas à gauche du CCD
#define OFFSET_CAPTEUR_BAS_HORI_CM -2.4 //Valeur à calibrer
//Distance entre le capteur spectral bas selon la verticale au coin en bas à gauche du CCD
#define OFFSET_CAPTEUR_BAS_VERTI_CM -0.2
#define FOCALE_M 0.3
#define DISTANCE_OBJCCD_MIN_M 0 //A calibrer, sert de réf (butée loin du CCD) = 0
#define DISTANCE_OBJCCD_MAX_M 0 //A calibrer
#define DISTANCE_PAS_MISE_AU_POINT_M 0 //A calibrer

class Fenetre : public QWidget
{
	Q_OBJECT

private:
	QPushButton* autoFocus;
	QPushButton* miseAuPointManuelleStartBtn;
	QPushButton* miseAuPointManuelleStopBtn;
	QPushButton* goButton;
	QPushButton* stopButton;
	QPushButton* photoButton;
	QPushButton* doGoto;
	QProgressBar* bar;
	QTabWidget* onglets;
	FocusWindow* focuswindow;
	QSpinBox* image_v;
	QSpinBox* image_h;
	QSpinBox* targetH;
	QSpinBox* targetV;
	QDoubleSpinBox* focusDistance;
	QDoubleSpinBox* sensorIntTime;
	QSlider* intTimeSlider;
	QLabel* label_v;
	QLabel* label_h;
	QLabel* label_spectre;
	QLabel* label_ref;
	QLabel* label_focus;
	QCheckBox* spectreActif;
	QCheckBox* redoRef;
	QCheckBox* startHere;
	QGroupBox* createFirstExclusiveGroup();
	QGroupBox* createSecondExclusiveGroup();
	QWidget* miseAuPoint;
	QWidget* run;
	QWidget* cameraSpecs;
	QWidget* sensorSettings;
	//QPushButton* zoneSelect;
	QGroupBox* polarizationGroupBox;
	QRadioButton* polarizationChoice[5];
	QPushButton* setSensorSettingsBtn;
	QPushButton* doRef;
	QComboBox* gain;
	QComboBox* upSensorList;
	QComboBox* downSensorList;

	//Onglet cam specs
	QSpinBox* camResHpx;
	QSpinBox* camResVpx;
	QDoubleSpinBox* pxHSize;
	QDoubleSpinBox* pxVSize;
	QPushButton* camSpecsValidateBtn;

	//ZoneSelection* m_zoneSelection;

	CapteurSpectral* capteurHaut;
	CapteurSpectral* capteurBas;

	int nbPhotoH;
	int nbPhotoV;
	int nbrPhoto;

	int nbCranPasH;
	int nbCranPasV;

	int nbCranMMH;
	int nbCranMMV;

	int absMaxPhotoH;
	int absMaxPhotoV;

	int maxPhotoH;
	int maxPhotoV;

	int resHpx;
	int resVpx;

	int totalPixels;

	double sizeHpx;
	double sizeVpx;

	double decalageHorizontalPx;
	double decalageVerticalPx;
	double decalageHorizontal;
	double decalageVertical;

	int mmCountH;
	int mmCountV;

	void Fenetre::closeEvent(QCloseEvent* event) override;
	void setCameraSpecs();
	void takeSpectralInfo();
	void takeGigaPixelPhoto();

	bool m_saveSpectrumInfo;

	bool do_stop;
	bool taking_photo;
	bool sensorSet;
	bool refOK;

	int startCoordPasH;
	int startCoordPasV;
	//int startCoordCransH;
	//int startCoordCransV;

	int currResPicH;
	int currResPicV;

	void deletePreviousFile(QString sensorType);
	
	QMutex stop_mutex;
	QMutex photo_mutex;
	QMutex buttonEnable_mutex;
	QMutex mutex_mmH;
	QMutex mutex_mmV;

	QString dataDir;

	WCam* w;

public:
	Fenetre();
	~Fenetre();
	void Log(std::string strMsg);
	SerialCommunication* serialcomm;
	void getSpecData(bool precise = false);
	void saveSpecData(QString t_data, QString t_sensor);
public slots:
	//void setStartingCoordAndBounds(QPoint, QPoint);
	//void resetStartingCoordsAndBounds();
	void updateProgressBar();

private slots:
	void disableButton();
	void enableButton();
	void assign_h();
	void assign_v();
	//void compteur();
	void start();
	void stop();
	void setCamSpecsBtnPress();
	//void openZoneSelect();
	//void setCaptureBtn(bool);
	//void setZonePic();
	void setIntTimeValue(int);
	void setSensorSettings();
	void refreshSensorsList();
	void miseAuPointSemiAuto();
	void miseAuPointManuelleStart();
	void miseAuPointManuelleStop();
	void doXYRef();
	void doGotoXY();
	void updateMaxPhotos(int stateChange);
	void updateStartCoords();

signals:
	//void getSelZone();
};

#endif // FENETRE_H
