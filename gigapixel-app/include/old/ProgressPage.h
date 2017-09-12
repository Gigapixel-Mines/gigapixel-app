// Page pour montrer l'avancement (photos prises/photos restantes)

#ifndef PROGRESSPAGE_H
#define PROGRESSPAGE_H

#include <QGridLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QWidget>
#include <QWizardPage>

class ProgressPage :  public QWizardPage
{

    Q_OBJECT

private:
	
    QProgressBar * bar;

public slots:
    void incNbPicturesTaken();
    void handleGoButtonClicked();
	void handleStopButtonClicked();

public:
	QPushButton * goButton;
	QPushButton * stopButton;
    ProgressPage();
    QPushButton* getStopButton();
	QPushButton* getGoButton();
	// for tests
	void Log(std::string strMsg);

signals:
	void goRequest();
    void stopRequest();

};

#endif // PROGRESSPAGE_H
