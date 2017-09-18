#ifndef CAPTEUR_SPECTRAL
#define CAPTEUR_SPECTRAL

#include <QtSerialPort>
#include <QWidget>

class CapteurSpectral : public QWidget
{
	// Comment this when on release, uncomment when debug
	Q_OBJECT

private:
	QString m_serialPortName; // utile ???
	QSerialPort* m_serialPort;
	QByteArray m_writeData;
	QByteArray m_readData;
	QTextStream m_standardOutput;
	qint64 m_bytesWritten;
	//QTimer          m_timer; // utile ???

	// Se connecter a un port (appele dans constructeur)
	// returns true if no problem (pas tres utile en vrai)
	bool connectSerialPort(QString t_serialNumber);

	//void handleReadyRead();

	bool m_connected;

	QString string_readData;

	public slots:

	// Lecture
	//void handleReadyRead();

public:
	// Constructor and destructor
	//CapteurSpectral();
	CapteurSpectral(QString t_serialNumber);
	virtual ~CapteurSpectral();
	void Log(std::string strMsg);
	bool is_connected();
	bool dataAvailable();
	bool contains(QString compareString);
	bool setUp(int intTime, int gain);
	void write(QByteArray);
	QString getData();

signals:

};

#endif // !CAPTEUR_SPECTRAL
