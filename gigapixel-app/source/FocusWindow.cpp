#include "AsynchronousGrab.h"
#include "FocusWindow.h"

#include <QApplication>
#include <QBuffer>
#include <QFile>
#include <QFileDialog>
#include <QGridLayout>
#include <QObject>
#include <QTextStream>

void FocusWindow::Log(std::string strMsg)
{
	// UNCOMMENT TO DEBUG
	QString filename = "Data.txt";
	QFile file(filename);
	if (file.open(QIODevice::ReadWrite | QIODevice::Append))
	{
		QTextStream stream(&file);
		stream << QString::fromUtf8(strMsg.c_str()) << endl;
	}
}


// Slot
void FocusWindow::OnImageReceived(QImage* image)
{
	Log("OnImageReceived called");
	label->setPixmap(QPixmap::fromImage(*image).scaled(label->size(), Qt::KeepAspectRatio));
	m_img = image;
	Log("Image actualisee !");
}

bool FocusWindow::SaveImage()
{
	Log("call on FocusWindow::saveImage()");
	if (!sync->m_bIsStreaming) //Si la caméra n'est pas allumée
	{
		sync->OnBnClickedButtonStartstop(); //On tente de l'allumer
	}
	if (sync->m_bIsStreaming) //On vérifie
	{
		//sync->OnBnClickedButtonStartstop();
		++nb_photos;
		Log("Saving image");
		QString imgpath = dirpath + "/photo" + QString::number(nb_photos) + ".png";
		Log(imgpath.toStdString());

		QPixmap pixmap = QPixmap::fromImage(*m_img);
		QSize bla = pixmap.size();
		Log(std::to_string(bla.height()));
		Log(std::to_string(bla.width()));
		bool res = pixmap.save(imgpath, "PNG");
		if (res)
		{
			Log("Sauvegarde reussie");
			return true;
			//emit PictureTaken();
			//Log("Emis");
		}
		else
		{
			Log("Sauvegarde echouee");
			return false;
		}
		//sync->OnBnClickedButtonStartstop();
	}
	else //Erreur
	{
		Log("Unable to start camera");
		return false;
	}
}

void FocusWindow::resetNbPhoto()
{
	nb_photos = 0;
}

// Default Constructor
FocusWindow::FocusWindow()
{
	setFixedSize(533, 400);
	setWindowTitle("Apercu du capteur");
	label = new QLabel;
	m_img = new QImage;
	sync = new AsynchronousGrab();
	Log("Sync object created");
	QObject::connect(sync, SIGNAL(ImageReceivedSignal(QImage*)), this, SLOT(OnImageReceived(QImage*)));

	label->setFixedHeight(400);
	label->setFixedWidth(533);

	this->show();
	// Prepare for saving
	nb_photos = 0;
	dirpath = QFileDialog::getExistingDirectory(this, tr("Dossier de stockage des images"), "C:/", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	sync->OnBnClickedButtonStartstop();
	//emit CameraStarted();
}

FocusWindow::~FocusWindow()
{
	if (sync->m_bIsStreaming)
	{
		sync->OnBnClickedButtonStartstop(); // close camera
	}
	delete sync;
	delete m_img;
}
