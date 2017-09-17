#include "CapteurSpectral.h"

#include <QtSerialPort>

#include <QCoreApplication>
#include <QFile>
#include <QObject>
#include <QStringList>
#include <QString>
#include <QTextStream>

#include <iostream> // TODO: remove after tests
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

bool CapteurSpectral::connectSerialPort()
{

	Log("calling SerialCommunication::connectSerialPort()");

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
			QString portDesc = serialPortInfo.description();
			//qDebug() << portDesc;
			if (portDesc.contains("Arduino"))
			{
				m_serialPort->setPort(serialPortInfo);
				m_serialPort->setBaudRate(QSerialPort::Baud9600);
				Log("Connection port serie etablie");
				return true;
			}
		}
	}

	m_standardOutput << "No serial port found" << endl;
	return false;
}

// Default constructor, inherits QWidget()
CapteurSpectral::CapteurSpectral()
	: QWidget()
	, m_standardOutput(stdout)
	, m_bytesWritten(0)
	, m_connected(false)
{
	Log("SerialCommunication constructor called");

	m_serialPortName = "Arduino";
	m_serialPort = new QSerialPort(this);
	connectSerialPort();

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
	Log("calling SerialCommunication::write");

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

bool CapteurSpectral::check(const char t_char)
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
		std::string string_readData(m_readData.constData(), m_readData.length());
		if (string_readData.compare(&t_char) == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		m_standardOutput << "Error or timeout while waiting for serial port answer" << endl;
		return false;
	}
}
