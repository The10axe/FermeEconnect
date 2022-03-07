#ifndef BDD_H
#define BDD_H

#include <QtSql>
#include <QObject>
#include <QTimer>

class bdd : public QWidget
{
    Q_OBJECT
public:
    bdd(); // Constructeur
    ~bdd(); // Destructeur
    void getProperties(); // Permet de charger les paramètres
    void updateProperties(long diametre, long longueur, float volume, int pourcentMin, int pourcentMax); // Permet de mettre �  jour les paramètres
    QList<QList<QString>> getLastDays(); // Permet d'obtenir les données des 7 derniers jours
    double lastTwoSouple(); // Permet de connaitre le débit entre les deux cuves souples
    bool detectFuite();

private:
    QSqlDatabase *base;
    QTimer *delay;

signals:
    void received(QString); // Permet l'envoie des éléments émis

private slots:
    void getLastLine(); // Permet de régulièrement récupéré la dernière ligne pour mise �  jour
};

#endif // BDD_H
