#ifndef FOCUSWINDOW_H
#define FOCUSWINDOW_H

#include "AsynchronousGrab.h"

#include <QLabel>
#include <QMutex>
#include <QPushButton>
#include <QWidget>

class FocusWindow : public QWidget
{
	Q_OBJECT 

	public:
		FocusWindow();
        virtual ~FocusWindow();
		void Log(std::string strMsg);
		bool SaveImage();
		void resetNbPhoto();
		QString setPolarization(int t_polarization);
		void stopImgRefresh(bool stop); //avoid threading problems potentially

	public slots:
		void OnImageReceived(QImage* image);

	private:
		QLabel* label;
		AsynchronousGrab* sync;
		QString dirpath;
		QString polarizationDir;
		int nb_photos;
		QImage* m_img;
		int polarization;

	signals:
		void PictureTaken();
		//void CameraStarted();
};

#endif // FOCUSWINDOW_H
