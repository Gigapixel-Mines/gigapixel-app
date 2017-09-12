#ifndef FENETRE_H
#define FENETRE_H

#include "FocusWindow.h"
#include "SerialCommunication.h"

#include <QCamera>
#include <QCameraViewfinder>
#include <QCameraImageCapture>
#include <QGridLayout>
#include <QLabel>
#include <QObject>
#include <QPushButton>
#include <QVideoWidget>
#include <QWidget>
#include <QtWidgets>

class Fenetre : public QWidget
{
	Q_OBJECT

private:
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
	QLabel* label_v;
	QLabel* label_h;
	QGroupBox* createFirstExclusiveGroup();
	QGroupBox* createSecondExclusiveGroup();

	int  alpha;
	int beta;
	int pas_h;
	int pas_v;
	int nbrPhoto;
	int gamma;

	void Fenetre::closeEvent(QCloseEvent* event) override;

public:
	Fenetre();
	~Fenetre();
	void Log(std::string strMsg);
	SerialCommunication* serialcomm;

private slots:
	void disableButton();
	void ableButton();
	void assign_h();
	void assign_v();
	void compteur();
	void stop();

signals:
	void LigneFinished();
	void PasGauche();
	void PasDroite();
	void FinCycle();
};

#endif // FENETRE_H
