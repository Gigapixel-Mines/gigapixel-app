#include "wcam.h"
#include <QCameraInfo>
#include <QMessageBox>




WCam::WCam()
{
    a = 0;
    this->setWindowTitle("Webcam");
    QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
       foreach (const QCameraInfo &cameraInfo, cameras) {
          if (cameraInfo.description() == "B525 HD Webcam")
          { a = a+1;
            layout = new QGridLayout();
            viewfinder = new QCameraViewfinder(this);
            viewfinder->setAspectRatioMode(Qt::KeepAspectRatio);
            layout->addWidget(viewfinder);

            camera = new QCamera(cameraInfo);

            camera->setViewfinder(viewfinder);
            imageCapture = new QCameraImageCapture(camera);

            camera->setCaptureMode(QCamera::CaptureStillImage);
            camera->start();
            this->setLayout(layout);
            show();}
          else
       {

        }
}
       if (a == 0 ){
                  error = new QMessageBox();
                  error->setWindowTitle("Erreur");
                  error->setText("Impossible de se connecter à la webcam");
                  error->exec();
       }



}

