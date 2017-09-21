#include "wcam.h"

#include <QDir>
#include <QCameraInfo>
#include <QDebug>
#include <QMessageBox>

WCam::WCam()
{
	a = 0;
	this->setWindowTitle("Webcam");
	QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
	foreach(const QCameraInfo &cameraInfo, cameras)
	{
		QString camDesc = cameraInfo.description();
		if (camDesc.contains("Logitech")) //"Logitech B525 HD Webcam" dans ma version
		{
			a = a + 1;
			layout = new QGridLayout();
			viewfinder = new QCameraViewfinder(this);
			viewfinder->setAspectRatioMode(Qt::KeepAspectRatio);
			layout->addWidget(viewfinder);

			camera = new QCamera(cameraInfo);

			camera->setViewfinder(viewfinder);
			imageCapture = new QCameraImageCapture(camera);
			imageCapture->setCaptureDestination(QCameraImageCapture::CaptureToFile);

			connect(imageCapture, SIGNAL(imageCaptured(int, QImage)), this, SLOT(processCapturedImage(int, QImage)));
			connect(imageCapture, SIGNAL(imageSaved(int, QString)), this, SLOT(imageSaved(int, QString)));

			connect(imageCapture, SIGNAL(readyForCaptureChanged(bool)), this, SLOT(readyForCapture(bool)));

			camera->setCaptureMode(QCamera::CaptureStillImage);
			camera->start();
			this->setLayout(layout);
			show();
		}
	}
	if (a == 0)
	{
		error = new QMessageBox();
		error->setWindowTitle("Erreur");
		error->setText("Impossible de se connecter à la webcam");
		error->exec();
	}
}

//void WCam::readyForCapture(bool ready)
//{
//	emit canCapture(ready);
//}

//QImage WCam::getZonePic()
//{
//	return selZonePic;
//}

//void WCam::imageSaved(int id, const QString& fileName)
//{
//	//qDebug() << fileName;
//	//selZonePic = QImage(fileName);
//	emit selZonePicReady();
//}

//void WCam::getImage()
//{
//	//qDebug() << imageCapture->isReadyForCapture();
//	//qDebug() << QDir::currentPath();
//	imageCapture->capture(QDir::currentPath() + QString("/selzone.jpg"));
//}

//void WCam::processCapturedImage(int requestId, const QImage& img)
//{
//	Q_UNUSED(requestId);
//	selZonePic = img;
//	if (!selZonePic.isNull())
//	{
//		//emit selZonePicReady();
//	}
//}