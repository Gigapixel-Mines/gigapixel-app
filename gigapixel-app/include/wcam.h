#ifndef WCAM_H
#define WCAM_H

#include <QCamera>
#include <QCameraViewfinder>
#include <QCameraImageCapture>
#include <QGridLayout>
#include <QImage>
#include <QLabel>
#include <QMessageBox>
#include <QPixmap>

class WCam : public QWidget
{
	Q_OBJECT

private:
	QGridLayout * layout;
	QCamera * camera;
	QCameraViewfinder * viewfinder;
	QCameraImageCapture * imageCapture;
	QMessageBox * error;
	QLabel* label;
	QImage selZonePic;
	int * a;

private slots:
	//void processCapturedImage(int requestId, const QImage& img);
	//void imageSaved(int id, const QString &fileName);
	//void readyForCapture(bool);

public:
	WCam();
	//QImage getZonePic();

signals:
	//void canCapture(bool);
	//void selZonePicReady();

public slots:
	//void getImage();

};

#endif // WCAM_H
