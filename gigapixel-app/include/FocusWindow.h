#ifndef FOCUSWINDOW_H
#define FOCUSWINDOW_H

#include "AsynchronousGrab.h"

#include <QLabel>
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

	public slots:
		void OnImageReceived(QImage* image);

	private:
		QLabel* label;
		AsynchronousGrab* sync;
		QString dirpath;
		int nb_photos;
		QImage* m_img;

	signals:
		void PictureTaken();
		//void CameraStarted();
};

#endif // FOCUSWINDOW_H
