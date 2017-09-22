// In main programm

#include "SerialCommunication.h"

#include <QtSerialPort>

#include <QCoreApplication>
#include <QFile>
#include <QMessageBox>
#include <QObject>
#include <QStringList>
#include <QString>
#include <QTextStream>
#include <QElapsedTimer>

#include <iostream>
#include <string>
#include <Windows.h>

using namespace std;

void SerialCommunication::Log(std::string strMsg)
{
	QString filename = "Data.txt";
	QFile file(filename);
	(file.open(QIODevice::ReadWrite | QIODevice::Append));
	{
		QTextStream stream(&file);
		stream << QString::fromUtf8(strMsg.c_str()) << endl;
	}
}

void SerialCommunication::setCransH(int t_value)
{
	cransPasH = t_value;
}

void SerialCommunication::setCransV(int t_value)
{
	cransPasV = t_value;
}

bool SerialCommunication::connectSerialPort()
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
SerialCommunication::SerialCommunication()
	: QWidget()
	, m_standardOutput(stdout)
	, m_bytesWritten(0)
	, m_connected(false)
	, string_readData()
	//, readData_mutex()
	//, data_received(false)
{
	Log("SerialCommunication constructor called");

	m_serialPortName = "Arduino";
	m_serialPort = new QSerialPort(this);
	connectSerialPort();

	if (!m_serialPort->open(QIODevice::ReadWrite))
	{
		Log("Failed to OPEN serial port");
		QMessageBox* error = new QMessageBox();
		error->setWindowTitle("Erreur");
		error->setText("Erreur lors de la connexion à l'Arduino");
		error->exec();
		m_connected = false;
	}
	else
	{
		Log("Serial port opened");
		m_serialPort->setDataTerminalReady(true);
		m_connected = true;
	}
	QObject::connect(m_serialPort, SIGNAL(errorOccured(QSerialPort::SerialPortError)), this, SLOT(printError(QSerialPort::SerialPortError)));
	//QObject::connect(m_serialPort, &QSerialPort::readyRead, this, &SerialCommunication::handleReadyRead);
	// pour la lecture
	//QObject::connect(m_serialPort, &QSerialPort::readyRead, this, &SerialCommunication::handleReadyRead);

	//For tests
	//Log("Message a sent");
	//write("a");

}

// Default destructor
SerialCommunication::~SerialCommunication()
{
	m_serialPort->close();
	delete m_serialPort;
}

void SerialCommunication::write(QByteArray c)
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


// Ecriture - higher-level functions

bool SerialCommunication::miseAuPointAuto(int nbDePasAFaire)
{
	Log("calling SerialCommunication::miseAuPointAuto()");
	write("e");
	if (dataAvailable())
	{
		if (!check('e'))
		{
			Log("Réponse invalide pour miseAuPointAuto");
			return false;;
		}
		else
		{
			return true;
		}
	}
	else
	{
		Log("Erreur, timeout pour mise au point auto");
		return false;
	}
}

void SerialCommunication::miseAuPointManuelleStart()
{
	Log("calling SerialCommunication::miseAuPointManuelleStart()");
	write("f");
}

void SerialCommunication::miseAuPointManuelleStop()
{
	Log("calling SerialCommunication::miseAuPointManuelleStop");
	write("g");
}

//Déplacement du capteur au point initial pour prendre des photos

bool SerialCommunication::findXYRef()
{
	Log("calling SerialCommunication::findXYRef()");
	write("h"); //Ask Arduino to find XY Ref
	//Wait for his answer
	if (dataAvailable(120000))
	{
		if (!check('h'))
		{
			Log("Réponse invalide");
			return false;
		}
	}
	Log("XY Ref OK");
	return true;
}

bool SerialCommunication::is_connected()
{
	return m_connected;
}

//Déplacement du capteur d'un pas, dans une direction, dans un sens

bool SerialCommunication::gauche()
{
	Log("calling SerialCommunication::gauche()");
	write("c");
	return true;
}

bool SerialCommunication::droite()
{
	Log("calling SerialCommunication::droite()");
	write("d");
	return true;
}

bool SerialCommunication::haut()
{
	Log("calling SerialCommunication::avanceVertical()");
	write("b");
	return true;
}

bool SerialCommunication::bas()
{
	Log("calling SerialCommunication::reculeVertical()");
	write("a");
	return true;
}

//bool SerialCommunication::dataAvailable(int timeout_ms)
//{
//	Sleep(250);
//	if (!m_connected)
//	{
//		Log("Error, serial port not connected, aborting read and check");
//		return false;
//	}
//	int retry = 0;
//	do
//	{
//		Log("DataAvailable try #" + std::to_string(retry));
//		if (m_serialPort->bytesAvailable() == 0)
//		{
//			if (m_serialPort->waitForReadyRead(timeout_ms))
//			{
//				m_readData = m_serialPort->readAll();
//				//m_serialPort->clear(QSerialPort::Input);
//				m_serialPort->flush();
//				string_readData.clear();
//				string_readData.append(m_readData.constData());
//				return true;
//			}
//			//else
//			//{
//			//	m_standardOutput << "Error or timeout while waiting for serial port answer" << endl;
//			//	return false;
//			//}
//		}
//		else if (m_serialPort->bytesAvailable() > 0)
//		{
//			m_readData = m_serialPort->readAll();
//			m_serialPort->flush();
//			m_serialPort->clear(QSerialPort::Input);
//			string_readData.clear();
//			string_readData.append(m_readData.constData());
//			return true;
//		}
//		++retry;
//	} while (retry <= 1);
//	m_standardOutput << "Error or timeout while waiting for serial port answer" << endl;
//	return false;
//}

bool SerialCommunication::dataAvailable(int timeout_ms)
{
	if (!m_connected)
	{
		Log("Error, serial port not connected, aborting read and check");
		return false;
	}
	QElapsedTimer timeoutWatchdog;
	timeoutWatchdog.start();
	while (m_serialPort->bytesAvailable() <= 0 && timeoutWatchdog.elapsed() < timeout_ms)
	{
		m_serialPort->waitForReadyRead(1);
	}
	if (m_serialPort->bytesAvailable() > 0)
	{
		m_readData = m_serialPort->readAll();
		m_serialPort->flush();
		string_readData.clear();
		string_readData.append(m_readData.constData());
		return true;
	}
	else
	{
		Log("Error while waiting for data");
		return false;
	}
	m_standardOutput << "Error or timeout while waiting for serial port answer" << endl;
	return false;
}

//bool SerialCommunication::dataAvailable(int timeout_ms)
//{
//	if (!m_connected)
//	{
//		Log("Error, serial port not connected, aborting read and check");
//		return false;
//	}
//	bool waiting(true);
//	QElapsedTimer timeoutWatchdog;
//	timeoutWatchdog.start();
//	do
//	{
//		readData_mutex.lock();
//		if (data_received)
//		{
//			data_received = false;
//			waiting = false;
//			string_readData.clear();
//			string_readData.append(m_readData.constData());
//			readData_mutex.unlock();
//			return true;
//		}
//		readData_mutex.unlock();
//	} while (waiting && timeoutWatchdog.elapsed() < timeout_ms);
//	if (waiting)
//	{
//		Log("Serial port timedOut");
//		return false;
//	}
//	else //Cas impossible
//	{
//		return false;
//	}
//}

void SerialCommunication::envoieCranParPas()
{
	Log("Calling SerialCommunication::envoieCranParPas()");
	//Init du transfert
	write("i");
	if (dataAvailable())
	{
		if (!check('i'))
		{
			Log("Réponse invalide");
			return;
		}
	}
	//Si l'arduino répond 'i'
	int weakByteMask = 0x000000FF;
	char strongByte = cransPasH >> 8;
	char weakByte = cransPasH & weakByteMask;
	//Il faut envoyer les deux bytes
	//On envoie le premier
	write(QByteArray(1, strongByte));
	if (dataAvailable())
	{
		if (!check('i'))
		{
			Log("Réponse invalide");
			return;
		}
	}
	write(QByteArray(1, weakByte));
	if (dataAvailable())
	{
		if (!check('i'))
		{
			Log("Réponse invalide");
			return;
		}
	}
	//Pour le pas vertical
	write("j");
	if (dataAvailable())
	{
		if (!check('j'))
		{
			Log("Réponse invalide");
			return;
		}
	}
	//Si l'arduino répond 'k'
	strongByte = cransPasH >> 8;
	weakByte = cransPasH & weakByteMask;
	//Il faut envoyer les deux bytes
	//On envoie le premier
	write(QByteArray(1, strongByte));
	if (dataAvailable())
	{
		if (!check('j'))
		{
			Log("Réponse invalide");
			return;
		}
	}
	write(QByteArray(1, weakByte));
	if (dataAvailable())
	{
		if (!check('j'))
		{
			Log("Réponse invalide");
			return;
		}
	}
	Log("Envoie du nombres de crans par pas vertical et horizontal effectué");
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

bool SerialCommunication::check(const char t_char)
{
	//Call dataAvailable first before calling check
	if (string_readData.contains(t_char))
	{
		return true;
	}
	else
	{
		return false;
	}
}

char SerialCommunication::getChar()
{
	std::string temp = string_readData.toStdString();
	if (temp.size() > 0)
	{
		return temp.at(0);
	}
	else
	{
		return '\0';
	}
}

bool SerialCommunication::gotoXY(int absPasH, int absCransH, int absPasV, int absCransV)
{
	Log("SerialCommunication::gotoXY");
	write("k");
	if (dataAvailable())
	{
		if (!check('k'))
		{
			Log("Réponse invalide");
			return false;
		}
	}
	//Si l'Arduino répond k
	write(QByteArray(1, absPasH));
	if (dataAvailable())
	{
		if (!check('k'))
		{
			Log("Réponse invalide");
			return false;
		}
	}
	int weakByteMask = 0x000000FF;
	char strongByte = absCransH >> 8;
	char weakByte = absCransH & weakByteMask;
	//Il faut envoyer les deux bytes
	//On envoie le premier
	write(QByteArray(1, strongByte));
	if (dataAvailable())
	{
		if (!check('k'))
		{
			Log("Réponse invalide");
			return false;
		}
	}
	write(QByteArray(1, weakByte));
	if (dataAvailable())
	{
		if (!check('k'))
		{
			Log("Réponse invalide");
			return false;
		}
	}
	//Si l'Arduino répond k
	write(QByteArray(1, absPasV));
	if (dataAvailable())
	{
		if (!check('k'))
		{
			Log("Réponse invalide");
			return false;
		}
	}
	strongByte = absCransV >> 8;
	weakByte = absCransV & weakByteMask;
	//Il faut envoyer les deux bytes
	//On envoie le premier
	write(QByteArray(1, strongByte));
	if (dataAvailable())
	{
		if (!check('k'))
		{
			Log("Réponse invalide");
			return false;
		}
	}
	write(QByteArray(1, weakByte));
	if (dataAvailable())
	{
		if (!check('k'))
		{
			Log("Réponse invalide");
			return false;
		}
	}
	//On attend la réponse pour les déplacements
	//OK hori
	if (dataAvailable(120000))
	{
		if (!check('z'))
		{
			Log("Réponse invalide");
			return false;
		}
	}
	//OK verti
	if (dataAvailable(120000))
	{
		if (!check('z'))
		{
			Log("Réponse invalide");
			return false;
		}
	}

	//on attend la confirmation finale
	if (dataAvailable())
	{
		if (!check('k'))
		{
			Log("Réponse invalide");
			return false;
		}
	}
	Log("Goto OK");
	return true;
}

bool SerialCommunication::relativeCransXY(int directionH, int cransH, int directionV, int cransV)
{
	Log("SerialCommunication::relaiveCransXY");
	write("s");
	if (dataAvailable())
	{
		if (!check('s'))
		{
			Log("Réponse invalide");
			return false;
		}
	}
	//Si l'Arduino répond k
	write(QByteArray(1, directionH));
	if (dataAvailable())
	{
		if (!check('s'))
		{
			Log("Réponse invalide");
			return false;
		}
	}
	int weakByteMask = 0x000000FF;
	char strongByte = cransH >> 8;
	char weakByte = cransH & weakByteMask;
	//Il faut envoyer les deux bytes
	//On envoie le premier
	write(QByteArray(1, strongByte));
	if (dataAvailable())
	{
		if (!check('s'))
		{
			Log("Réponse invalide");
			return false;
		}
	}
	write(QByteArray(1, weakByte));
	if (dataAvailable())
	{
		if (!check('s'))
		{
			Log("Réponse invalide");
			return false;
		}
	}
	//Si l'Arduino répond k
	write(QByteArray(1, directionV));
	if (dataAvailable())
	{
		if (!check('s'))
		{
			Log("Réponse invalide");
			return false;
		}
	}
	strongByte = cransV >> 8;
	weakByte = cransV & weakByteMask;
	//Il faut envoyer les deux bytes
	//On envoie le premier
	write(QByteArray(1, strongByte));
	if (dataAvailable())
	{
		if (!check('s'))
		{
			Log("Réponse invalide");
			return false;
		}
	}
	write(QByteArray(1, weakByte));
	if (dataAvailable())
	{
		if (!check('s'))
		{
			Log("Réponse invalide");
			return false;
		}
	}
	//On attend la réponse pour les déplacements
	//OK hori
	if (dataAvailable(120000))
	{
		if (!check('z'))
		{
			Log("Réponse invalide");
			return false;
		}
	}
	//OK verti
	if (dataAvailable(120000))
	{
		if (!check('z'))
		{
			Log("Réponse invalide");
			return false;
		}
	}

	//on attend la confirmation finale
	if (dataAvailable())
	{
		if (!check('s'))
		{
			Log("Réponse invalide");
			return false;
		}
	}
	Log("Relative XY OK");
	return true;
}

//bool SerialCommunication::enableSpecPos(bool enable)
//{
//	if (enable)
//	{
//		write("l");
//		if (dataAvailable())
//		{
//			if (check('l'))
//			{
//				return true;
//			}
//			else
//			{
//				Log("Invalid answer");
//				return false;
//			}
//		}
//		else
//		{
//			Log("Request Timed Out");
//			return false;
//		}
//	}
//	else
//	{
//		write("m");
//		if (dataAvailable())
//		{
//			if (check('m'))
//			{
//				return true;
//			}
//			else
//			{
//				Log("Invalid answer");
//				return false;
//			}
//		}
//		else
//		{
//			Log("Request Timed Out");
//			return false;
//		}
//	}
//}

bool SerialCommunication::enablePolarization(bool enable)
{
	if (enable)
	{
		write("n");
		if (dataAvailable(20000))
		{
			if (check('n'))
			{
				return true;
			}
			else
			{
				Log("Invalid answer");
				return false;
			}
		}
		else
		{
			Log("Request Timed Out");
			return false;
		}
	}
	else
	{
		write("o");
		if (dataAvailable(20000))
		{
			if (check('o'))
			{
				return true;
			}
			else
			{
				Log("Invalid answer");
				return false;
			}
		}
		else
		{
			Log("Request Timed Out");
			return false;
		}
	}
}

bool SerialCommunication::selectPolarization(int polarization)
{
	int polaMask = 0x000000FF;
	char polaByte = polarization & polaMask;
	write("p");
	write(QByteArray(1, polaByte));
	if (dataAvailable(50000))
	{
		if (check('p'))
		{
			return true;
		}
		else
		{
			Log("Invalid answer");
			return false;
		}
	}
	else
	{
		Log("Request Timed Out");
		return false;
	}
}

void SerialCommunication::printError(QSerialPort::SerialPortError error)
{
	Log("Serial Port Error");
	qDebug() << error;
}

//void SerialCommunication::handleReadyRead()
//{
//	readData_mutex.lock();
//	data_received = true;
//	m_readData = m_serialPort->readAll();
//	m_serialPort->flush();
//	readData_mutex.unlock();
//}
