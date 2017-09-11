#ifndef WCAM_H
#define WCAM_H
#include <QGridLayout>
#include <QCamera>
#include <QCameraViewfinder>
#include <QCameraImageCapture>
#include <QMessageBox>


class WCam : public QWidget
{
	Q_OBJECT

private:
	QGridLayout * layout;
	QCamera * camera;
	QCameraViewfinder * viewfinder;
	QCameraImageCapture * imageCapture;
	QMessageBox * error;
	int * a;


public:
	WCam();

};

#endif // WCAM_H
