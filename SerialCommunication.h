// In main programm

#ifndef SERIALCOMMUNICATION_H
#define SERIALCOMMUNICATION_H

#include <QtSerialPort>
#include <QWidget>

class SerialCommunication : public QWidget
{
	// Comment this when on release, uncomment when debug
	Q_OBJECT

private:
	QString         m_serialPortName; // utile ???
	QSerialPort     *m_serialPort;
	QByteArray      m_writeData;
	QByteArray      m_readData;
	QTextStream     m_standardOutput;
	qint64          m_bytesWritten;
	//QTimer          m_timer; // utile ???

	// Se connecter a un port (appele dans constructeur)
	// returns true if no problem (pas tres utile en vrai)
	bool connectSerialPort();

	// Ecriture - low-level
	void write(QByteArray);

	public slots:

	void emergencyStop();
	// "s"
	void miseAuPointAv();

	void miseAuPointAr();

	void miseAuPointStop();

	void initialPic();

	void droite();

	void gauche();

	void haut();

	void bas();



	// Lecture
	void handleReadyRead();

public:
	// Constructor and destructor
	SerialCommunication();
	virtual ~SerialCommunication();
	void Log(std::string strMsg);

signals:
	// Suite a la lecture
	void MvtFinished();
	void InitFinished();

};

#endif // SERIALCOMMUNICATION_H
