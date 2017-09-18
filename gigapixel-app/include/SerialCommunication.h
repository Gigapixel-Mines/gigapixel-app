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
    QString m_serialPortName; // utile ???
    QSerialPort* m_serialPort;
    QByteArray m_writeData;
    QByteArray m_readData;
    QTextStream m_standardOutput;
    qint64 m_bytesWritten;
	QString string_readData;
    //QTimer          m_timer; // utile ???

    // Se connecter a un port (appele dans constructeur)
    // returns true if no problem (pas tres utile en vrai)
    bool connectSerialPort();
	
    // Ecriture - low-level
    void write(QByteArray);
	//void handleReadyRead();
	bool m_connected;

	int cransPasH;
	int cransPasV;

	int coordPasH;
	int coordPasV;
	int coordCransH;
	int coordCransV;
	int maxPasH;
	int maxPasV;

public slots:

    //void emergencyStop(); pas implémenté
    // "s"
    void miseAuPointAuto();
    void miseAuPointManuelleStart();
    void miseAuPointManuelleStop();

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
	bool dataAvailable();
	bool check(const char t_char);
	char getChar();
	void setMaxPasH(int);
	void setMmaxPasV(int);

signals:
    // Suite a la lecture
	void MvtFinished();
    //void InitFinished();
};

#endif // SERIALCOMMUNICATION_H
