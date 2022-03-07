#ifndef ARDUINO_H
#define ARDUINO_H

#include <QSerialPort>
#include <QObject>
#include <QTimer>

class Arduino : public QSerialPort
{
    Q_OBJECT // On a des slots et signal, donc c'est nécessaire
public:
    Arduino(); // Constructeur de l'arduino
    ~Arduino(); // Destructeur de l'arduino

private:
    QString PortCOM; // PortCOM choisis
    QString buffer; // Le buffer de la trame
    QTimer *delay; // Définition du timer pour faire des envois de message en attendant les accusée de réception
    bool ack; // Est-ce que nous avons eu un accusé de réception
    QList<QByteArray> trame; // Variable stockant les trames
    const int vendor_id = 10755;
    const int product_id = 67;

signals:
    void received(QString); // Signal qu'une trame a été reçu
    void waiting(bool); // Signal que l'on attend un accusé de réception

private slots:
    void read(); // Slot permettant de lire sur activité du port série
    void sendUntilAck(); // Slot permettant d'envoyer un message tant qu'un accusée de réception n'as pas été reçu

public slots:
    void send(QString, bool); // Slot public permettant d'envoyer une trame à l'arduino
};

#endif // ARDUINO_H
