// In main programm

#include "SerialCommunication.h"

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
		m_serialPort->setPort(QSerialPortInfo::availablePorts()[0]);
		m_serialPort->setBaudRate(QSerialPort::Baud9600);

	}

	Log("Connection port serie etablie");
	return true;

}

// Default constructor, inherits QWidget()
SerialCommunication::SerialCommunication()
	: QWidget()
	, m_standardOutput(stdout)
	, m_bytesWritten(0)
{
	Log("SerialCommunication constructor called");

	m_serialPortName = "Arduino";
	m_serialPort = new QSerialPort(this);
	connectSerialPort();

	if (!m_serialPort->open(QIODevice::ReadWrite))
	{
		Log("Failed to OPEN serial port");
	}
	else
	{
		Log("Serial port opened");
		m_serialPort->setDataTerminalReady(true);
	}

	// pour la lecture
	QObject::connect(m_serialPort, &QSerialPort::readyRead, this, &SerialCommunication::handleReadyRead);

	//For tests
	//Log("Message a sent");
	//write("a");

}

// Default destructor
SerialCommunication::~SerialCommunication()
{
	m_serialPort->close(); // utile ???
}

/*
// Ecriture - low-level
void SerialCommunication::write(std::string c){



    Log("calling SerialCommunication::write");

    // TODO: ouvrir le port ?
    //if (!m_serialPort->open(QIODevice::ReadWrite)) {
    //    m_standardOutput << QObject::tr("Failed to open port %1, error: %2").arg(m_serialPortName).arg(m_serialPort->errorString()) << endl;
    //    return;
    //}

	// QByteArray qba = QString::fromStdString(stdString).toAscii();
	//m_writeData = QString::fromStdString(c);
	QByteArray writeData(c.c_str(), c.length());
	
	//for tests
	std::string toDebugString(writeData.constData(), writeData.length());
	Log(toDebugString);

    qint64 bytesWritten = m_serialPort->write(writeData); // ecriture proprement dite

    // verification : message correctement transmis ?

    // verifier les problemes possibles
    if (bytesWritten == -1) {
		//Log(("Failed to write the data to port %1, error: %2").arg(m_serialPortName).arg(m_serialPort->errorString());
		Log("Failed to write the data");
		return;
	}
    else if (bytesWritten != m_writeData.size()) {
        //standardOutput << QObject::tr("Failed to write all the data to port %1, error: %2").arg(m_serialPortName).arg(m_serialPort->errorString()) << endl;
		Log("Failed to write all the data ");
		return;
	}
    else if (!m_serialPort->waitForBytesWritten(5000)) {
        //standardOutput << QObject::tr("Operation timed out or an error occurred for port %1, error: %2").arg(serialPortName).arg(serialPort->errorString()) << endl;
		Log("Operation timed out or an error occurred");
		return;
	}

    // si on arrive ici, c'est que tout s'est bien passe
    m_standardOutput << QObject::tr("Data successfully sent to port %1").arg(m_serialPortName) << endl;

    // TODO: fermer le port ???
    //m_serialPort->close();
    // choix a faire :
    // a chaque fois, ouvrir le port/ecrire/fermer, ou bien
    // ouvrir une fois pour toutes, ecrire les differents messages, fermer le port a la fin seulement
    // (question des performances ? i.e. du temps)

}
*/

// ci-dessous version fonctionnelle de secours (pas tres propre; "statique" ; tout code a la volee)

void SerialCommunication::write(QByteArray c)
{

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


// Ecriture - higher-level functions

void SerialCommunication::emergencyStop()
{
	Log("calling SerialCommunication::emergencyStop()");
	write("s");
}

/*void SerialCommunication::moveCameraTo(int x, int y){

	string debugMesg = ("calling SerialCommunication::goTo with args" + (std::to_string(x)) + (std::to_string(y)));
	Log(debugMesg);

    //TODO: fabriquer la bonne chaine de caracteres (pas sure de ce code-ci)
    const char* string_x = QByteArray::number(x); // WARNING "integer of different size"
    const char* string_y = QByteArray::number(y);

    write("b");
    write(string_x);
    write(string_y);

    // TODO: verifier protocole de communication avec elec

}

void SerialCommunication::startCycle() {
    Log("calling SerialCommunication::startCycle()");
    write("a");
}

void SerialCommunication::moveCameraToNextPosition() {
    Log("calling SerialCommunication::moveCameraToNextPosition()");
    write("o"); // "OK"
}*/

//Debut des fonctions utilisées par les VS 2016/2017.
//On est partis pour coder sur l'arduino des déplacements élémentaires, et gérer les cycles dans le programme C++.
//3 fonctions pour la fise au point : Mettre en marche le moteur dans un sens lorsqu'on appuie sur un bouton.
//Arrêter les moteurs dès qu'on relâche le bouton.

void SerialCommunication::miseAuPointAv()
{
	Log("calling SerialCommunication::miseAuPointAv()");
	write("e");
}

void SerialCommunication::miseAuPointAr()
{
	Log("calling SerialCommunication::miseAuPointAr()");
	write("f");
}

void SerialCommunication::miseAuPointStop()
{
	Log("calling SerialCommunication::miseAuPointStop()");
	write("g");
}

//Déplacement du capteur au point initial pour prendre des photos

void SerialCommunication::initialPic()
{
	Log("calling SerialCommunication::initialPic()");
	write("h");
}

//Déplacement du capteur d'un pas, dans une direction, dans un sens

void SerialCommunication::gauche()
{
	Log("calling SerialCommunication::avanceHorizontal()");
	write("c");
}

void SerialCommunication::droite(){
    Log("calling SerialCommunication::reculeHorizontal()");
    write("d");
}

void SerialCommunication::haut()
{
	Log("calling SerialCommunication::avanceVertical()");
	write("b");
}

void SerialCommunication::bas()
{
	Log("calling SerialCommunication::reculeVertical()");
	write("a");
}


// Lecture


void SerialCommunication::handleReadyRead()
{

	Log("Nouveau message recu : call on handleReadyRead()");
	m_readData = m_serialPort->readAll();
	m_serialPort->flush();
	Log("readAll() execute. Message lu :");
	std::string string_readData(m_readData.constData(), m_readData.length());
	Log(string_readData);
	Log(std::to_string(string_readData.compare("j")));
	Log(std::to_string(string_readData.compare("z")));
	//  m_standardOutput << std::to_string(string_readData.compare("z")) << endl ;
	if (string_readData.compare("j") == 0)
	{
		Sleep(250);
		emit InitFinished();
		Log("signal InitFinished() emis par SerialCommunication");
	}
	else if (string_readData.compare("z") == 0)
	{
		Sleep(250);
		emit MvtFinished();

		Log("signal MvtFinished() emis par SerialCommunication");
	}

	// on peut rajouter d'autres signaux...

}
