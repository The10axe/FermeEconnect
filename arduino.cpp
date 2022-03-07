#include "arduino.h"

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QObject>
#include <QTimer>

Arduino::Arduino()
{
    bool arduino_is_available = false; // On définie l'arduino trouvé à faux
    foreach (const QSerialPortInfo &serialPortInfo,QSerialPortInfo::availablePorts())
    { // On cherche l'arduino dans tout les ports série
        if (serialPortInfo.hasVendorIdentifier() && serialPortInfo.hasProductIdentifier())
        {
            if (serialPortInfo.vendorIdentifier() == vendor_id)
            {
                if (serialPortInfo.productIdentifier() == product_id)
                {
                 PortCOM = serialPortInfo.portName();
                 arduino_is_available = true; // Si on en trouve un, on dit qu'on en a trouvé
                }
            }
         }
     }

    if (arduino_is_available)
   { // Si un arduino est trouvé
          setPortName(PortCOM); // On s'y connecte
          open(QSerialPort::ReadWrite);
          setBaudRate(QSerialPort::Baud9600);
          setDataBits(QSerialPort::Data8);
          setFlowControl(QSerialPort::NoFlowControl);
          setParity(QSerialPort::NoParity);
          setStopBits(QSerialPort::OneStop);

          QObject::connect(this,SIGNAL(readyRead()),this,SLOT(read()));
    }
    delay = new QTimer(); // On créer un timer qui va nous servir pour les accusée de réception
    ack = true; // On considère que la réponse à été donnée
    connect(delay, SIGNAL(timeout()),this,SLOT(sendUntilAck())); // On lis le timer a la fonction gérent les accusée de récéption
}

void Arduino::read(){ // Fonction de lecture de trame jusqu'au "\n"
    char buf[1024]; // On initialise localement un buffer en char
    qint64 lineLength = readLine(buf, sizeof(buf)); // On lit la trame dans le buffer, cette fonction retourne le nombre de caractère lu
    if (lineLength != -1) { // Si il y a eu une lecture
        for(int i=0;i<lineLength;i++){ // Pour tout les caractères lu, on les mets dans le buffer QString
            buffer += QString(buf[i]);
        }
        if (buffer.endsWith("\n")){ // Si le buffer QString fini par "\n"
            buffer.remove("\r\n"); // On retire les fins de trame de l'arduino (\r\n)
            if ((buffer == "OK") && (ack == false)){ // Puis on regarde si c'est un accusée de reception (Le fameux OK) et si on en attends un
                trame.removeAt(0);
                ack = true;
            } else { // Sinon on emet le buffer reçu
                emit received(buffer);
            }
            buffer = ""; // On vide le buffer pour préparé le prochain message
        }
    }
}

void Arduino::send(QString data, bool waitAck){ // Fonction permettant d'envoyer une trame avec ou sans accuser de réception
    data = data + '\n'; // On ajouter un "\n" à la trame (on utilisera readStringUntil(\n); sur Arduino)
    trame.append(data.toUtf8()); // On ajoute le message à envoyer à la liste de trame
    if (waitAck){ // Si on doit attendre un accusé de réception
        ack = false; // On déclare que l'on en attends un
        delay->start(100); // On démarre le minuteur qui va essayé toute les 100ms d'envoyer la trame en attente de l'accusée de réception
    } else {
        write(trame[0]); // Sinon on écrit la trame en brute
    }
}

void Arduino::sendUntilAck(){
    emit waiting(ack); // On émets un signal permettant de désactivé les boutons de l'IHM en attendant l'accusée de réception
    if (ack == false){ // Si il n'y a pas eu d'accusé de réception
        write(trame[0]); // On envoi la trame
    } else { // Sinon
        if (trame.length()>0){ // Si il reste une trame à envoyer, on remets l'accusé de réception à faux
            ack = false;
        } else { // Sinon on arrête le minuteur
            delay->stop();
        }
    }
}

Arduino::~Arduino(){ // En cas de destruction, il faut s'assurée de la fermeture du ports, le reste est géré par la classe hérité.
    close();
}
