#include "CapteurSpectral.h"

#include <QtSerialPort>

#include <QCoreApplication>
#include <QFile>
#include <QObject>
#include <QStringList>
#include <QString>
#include <QTextStream>

#include <cstdlib>
#include <iostream>
#include <string>
#include <Windows.h>

using namespace std;

void CapteurSpectral::Log(std::string strMsg)
{
	QString filename = "Data.txt";
	QFile file(filename);
	(file.open(QIODevice::ReadWrite | QIODevice::Append));
	{
		QTextStream stream(&file);
		stream << QString::fromUtf8(strMsg.c_str()) << endl;
	}
}

bool CapteurSpectral::connectSerialPort(QString t_serialNumber)
{

	Log("calling CapteurSpectral::connectSerialPort()");
	
	int portCount = QSerialPortInfo::availablePorts().count();
	if (portCount == 0)
	{
		m_standardOutput << "No serial port found" << endl;
		return false;
	}
	else
	{
		QList<QSerialPortInfo> sPorts = QSerialPortInfo::availablePorts();
		foreach(const QSerialPortInfo& serialPortInfo, sPorts)
		{
			if (serialPortInfo.serialNumber() == t_serialNumber)
			{
				m_serialPort->setPort(serialPortInfo);
				m_serialPort->setBaudRate(QSerialPort::Baud115200);
				m_serialPort->setParity(QSerialPort::NoParity);
				m_serialPort->setStopBits(QSerialPort::OneStop);
				Log("Connexion port serie etablie");
				//Set up the sensor : bank mode, int time and gain
				return true;
			}
		}
	}

	m_standardOutput << "No serial port found" << endl;
	return false;
}

//CapteurSpectral::CapteurSpectral()
//{
//
//}

// Default constructor, inherits QWidget()
CapteurSpectral::CapteurSpectral(QString t_serialNumber)
	: QWidget()
	, m_standardOutput(stdout)
	, m_bytesWritten(0)
	, m_connected(false)
	, string_readData()
{
	Log("CapteurSpectral constructor called");

	m_serialPortName = "AMS";
	m_serialPort = new QSerialPort(this);
	connectSerialPort(t_serialNumber);

	if (!m_serialPort->open(QIODevice::ReadWrite))
	{
		Log("Failed to OPEN serial port");
		m_connected = false;
	}
	else
	{
		Log("Serial port opened");
		m_serialPort->setDataTerminalReady(true);
		m_connected = true;
		int compteur = 0;
		bool OK = false;
		do
		{
			++compteur;
			Log("Essai " + std::to_string(compteur) + " pour communiquer avec le capteur");
			write("AT\n"); //Test pour vérifier que le capteur répond
			while (!dataAvailable()); //Wait to get data
			OK = contains("OK");
		} while (!OK && compteur < 10); //check if the sensor answers OK
		if (OK)
		{
			Log("Capteur initialisé");
		}
		else
		{
			Log("Problème de communication avec le capteur");
		}
	}
}

// Default destructor
CapteurSpectral::~CapteurSpectral()
{
	m_serialPort->close();
	delete m_serialPort;
}

// ci-dessous version fonctionnelle de secours (pas tres propre; "statique" ; tout code a la volee)

void CapteurSpectral::write(QByteArray c)
{
	if (!m_connected)
	{
		Log("Serial port is not connected, aborting write");
		return;
	}
	Log("calling CapteurSpectral::write");

	QTextStream standardOutput(stdout);

	int portCount = QSerialPortInfo::availablePorts().count();

	if (portCount == 0)
	{
		Log("No serial port found");
		return;
	}

	//QSerialPort serialPort;
	//QString serialPortName = "Arduino";
	//m_serialPort->setPort(QSerialPortInfo::availablePorts()[0]);

	//->setBaudRate(QSerialPort::Baud9600);


	QByteArray writeData = c; //Caractere a choisir

	qint64 bytesWritten = m_serialPort->write(writeData);

	if (bytesWritten == -1)
	{
		Log("Failed to write the data");
		return;
	}
	else if (bytesWritten != writeData.size())
	{
		Log("Failed to write all the data to port");
		return;
	}
	else if (!m_serialPort->waitForBytesWritten(5000))
	{
		Log("Operation timed out or an error occurred for port");
		return;
	}

	Log("Data successfully sent to port %1");
	//serialPort.close();

	return;

}

bool CapteurSpectral::is_connected()
{
	return m_connected;
}

bool CapteurSpectral::dataAvailable()
{
	if (!m_connected)
	{
		Log("Error, serial port not connected, aborting read and check");
		return false;
	}
	if (m_serialPort->waitForReadyRead(5000))
	{
		m_readData = m_serialPort->readAll();
		m_serialPort->flush();
		string_readData.clear();
		string_readData.append(m_readData.constData());
		return true;
	}
	else
	{
		m_standardOutput << "Error or timeout while waiting for serial port answer" << endl;
		return false;
	}
}

QString CapteurSpectral::getData()
{
	if (dataAvailable())
	{
		return string_readData;
	}
	else
	{
		return QString("");
	}
}

// Lecture


//void SerialCommunication::handleReadyRead()
//{
//
//	Log("Nouveau message recu : call on handleReadyRead()");
//	m_readData = m_serialPort->readAll();
//	m_serialPort->flush();
//	Log("readAll() execute. Message lu :");
//	std::string string_readData(m_readData.constData(), m_readData.length());
//	Log(string_readData);
//	Log(std::to_string(string_readData.compare("j")));
//	Log(std::to_string(string_readData.compare("z")));
//	//  m_standardOutput << std::to_string(string_readData.compare("z")) << endl ;
//	if (string_readData.compare("j") == 0)
//	{
//		Sleep(250);
//		emit InitFinished();
//		Log("signal InitFinished() emis par SerialCommunication");
//	}
//	else if (string_readData.compare("z") == 0)
//	{
//		Sleep(250);
//		emit MvtFinished();
//
//		Log("signal MvtFinished() emis par SerialCommunication");
//	}
//
//	// on peut rajouter d'autres signaux...
//
//}

bool CapteurSpectral::contains(QString compareString)
{
	if (string_readData.contains(compareString))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CapteurSpectral::setUp(int intTime, int gain)
{
	write("ATTCSMD=2\n");
	if (dataAvailable())
	{
		if (contains("OK"))
		{
			Log("Banque 2 sélectionnée");
		}
		else
		{
			Log("Erreur lors de la sélection de la banque 2");
			return false;
		}
	}
	else
	{
		Log("Problème de communication avec le capteur");
		return false;
	}
	QString qGain = QString::number(gain);
	write("ATGAIN=" + qGain.toLocal8Bit() + "\n");
	if (dataAvailable())
	{
		if (contains("OK"))
		{
			Log("Gain envoyé");
		}
		else
		{
			Log("Erreur lors de la sélection de la banque 2");
			return false;
		}
	}
	else
	{
		Log("Problème de communication avec le capteur");
		return false;
	}
	QString qIntTime = QString::number(intTime);
	write("ATINTTIME=" + qIntTime.toLocal8Bit() + "\n");
	if (dataAvailable())
	{
		if (contains("OK"))
		{
			Log("IntTime envoyé");
		}
		else
		{
			Log("Erreur lors de la sélection de la banque 2");
			return false;
		}
	}
	else
	{
		Log("Problème de communication avec le capteur");
		return false;
	}
	return true;
}
