#include <QApplication>
#include <QPushButton>

#include <iostream> // for tests

// Pour le flux video de la webam
#include <QtMultimedia>
#include <QtMultimediaWidgets>
#include <QVideoWidget>
#include <QPainter>
#include <QLabel>

using namespace std;

int main ()
{
//Définition de l'image contenant le rectangle
QImage image = QImage(1200,675,QImage::Format_ARGB32);
QPainter painter(&image);

//Permet de détecter la webcam
QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    QWidget w;
    QWidget videoContainer;
    QCamera *camera;
    QVideoWidget* videoWidget;
    QLabel *myLabel;
    QGridLayout *layout;
    if (cameras.size() > 0){
            Log("Webcam trouvee");
            return 0;
    }
