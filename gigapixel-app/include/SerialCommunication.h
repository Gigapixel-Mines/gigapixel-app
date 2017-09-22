// In main programm

#ifndef SERIALCOMMUNICATION_H
#define SERIALCOMMUNICATION_H

#include <QMutex>
#include <QtSerialPort>
#include <QWidget>

class SerialCommunication : public QWidget
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
	QString string_readData;
	//QMutex readData_mutex;
    //QTimer          m_timer; // utile ???

    // Se connecter a un port (appele dans constructeur)
    // returns true if no problem (pas tres utile en vrai)
    bool connectSerialPort();
	
    // Ecriture - low-level
    void write(QByteArray);
	//void handleReadyRead();
	bool m_connected;
	//bool data_received;

	int cransPasH;
	int cransPasV;

	int maxPasH;
	int maxPasV;
//
//private slots:
//	void handleReadyRead();

public slots:

    //void emergencyStop(); pas implémenté
    // "s"
    bool miseAuPointAuto(int);
    void miseAuPointManuelleStart();
    void miseAuPointManuelleStop();
	void printError(QSerialPort::SerialPortError);

    // Lecture
    //void handleReadyRead();

public:
    // Constructor and destructor
	SerialCommunication();
    virtual ~SerialCommunication();
	void Log(std::string strMsg);
	void setCransH(int t_value);
	void setCransV(int t_value);
	void envoieCranParPas();
	bool findXYRef();
	bool is_connected();
	bool droite();
	bool gauche();
	bool haut();
	bool bas();
	bool dataAvailable(int timeout_ms = 5000);
	bool check(const char t_char);
	char getChar();
	bool gotoXY(int relPasH, int relCransH, int relPasV, int relCransV);
	//bool enableSpecPos(bool);
	bool enablePolarization(bool);
	bool selectPolarization(int);
	bool relativeCransXY(int directionH, int cransH, int directionV, int cransV);

signals:
    // Suite a la lecture
	void MvtFinished();
    //void InitFinished();
};

#endif // SERIALCOMMUNICATION_H
