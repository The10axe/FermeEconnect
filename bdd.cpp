#include "bdd.h"

#include <QtSql>
#include <QObject>
#include <QDebug>
#include <QTimer>

bdd::bdd()
{
    base = new QSqlDatabase(QSqlDatabase::addDatabase("QMYSQL")); // On créer une connexion en utilisant le driver QMySQL
    base->setHostName("192.168.0.101"); // On définit l'adresse de la base de données
    //base->setHostName("localhost");
    base->setPort(3308); // On définit le port de la base de données
    base->setUserName("root"); // On définit l'utilisateur de la base de données
    base->setPassword("Administrateur28!"); // On donne le mot de passe
    //base->setPassword("");
    base->setDatabaseName("fermeeconnect"); // On définit la base de données par défaut
    if(base->open()){} // On ouvre la base de donnée
    delay = new QTimer(); // On créer un timer
    connect(delay, SIGNAL(timeout()), this, SLOT(getLastLine())); // On affecte la fin du timer au slot getLastLine()
    delay->start(1000); // On démarre le timer pour toutes les secondes
}

bdd::~bdd(){
    base->close(); // On ferme la BDD
}

void bdd::getLastLine(){ // On obtient la dernière ligne
    QSqlQuery query(*base); // On fait une requête avec la BDD
    QString buffer = ""; // On charge un tampon vide qui permettra de prendre le résultat
    if(query.exec("SELECT volume_rigide,volume_souple,conso_inst,date FROM fermeeconnect.eau ORDER BY id DESC LIMIT 1")){ // On éxécute la commande SQL permettant de récupéré la dernière ligne.
        while(query.next()){ // Puis on lit la ligne
            for(int x=0; x<query.record().count(); x++){
               buffer += query.value(x).toString() + ";"; // Pour chaque valeur on l'ajoute au tampon en ajoutant un ";" derrière 
            }
        }
    }
    buffer.chop(1); // On retire le dernier ";"
    emit received(buffer); // On envoie le buffer
}

void bdd::getProperties(){ // On obtient les derniers paramètres des cuves
    QSqlQuery query(*base);
    QString buffer = "get;";
    if(query.exec("SELECT diametre,longueur,volume,pourcentmin,pourcentmax FROM fermeeconnect.properties ORDER BY id DESC LIMIT 1")){
        while(query.next()){
            for(int x=0; x<query.record().count(); x++){
               buffer += query.value(x).toString() + ";";
            }
        }
    }
    buffer.chop(1);
    emit received(buffer);
}

void bdd::updateProperties(long diametre, long longueur, float volume, int pourcentMin, int pourcentMax){ // Permet de mettres �  jour le paramètres des cuves
    QSqlQuery query(*base);
    if(query.exec("INSERT INTO fermeeconnect.properties (diametre, longueur, volume, pourcentmin ,pourcentmax) VALUES ("+QString::number(diametre)+", "+QString::number(longueur)+", "+QString::number(volume)+", "+QString::number(pourcentMin)+", "+QString::number(pourcentMax)+")")){
        getProperties();
    }
}

QList<QList<QString>> bdd::getLastDays(){ // Permets de charger les données des 7 derniers jours
    QSqlQuery query(*base);
    QList<QString> volumeCuveRigide;
    QList<QString> volumeCuveSouple;
    QList<QString> date;
    if(query.exec("SELECT volume_rigide,volume_souple,date FROM fermeeconnect.eau WHERE date >= DATE(NOW()) - INTERVAL 7 DAY ORDER BY id ASC;")){
        while(query.next()){
            for(int x = 0; x < query.record().count(); x++){
                if (query.record().fieldName(x) == "volume_rigide"){
                    volumeCuveRigide.append(query.value(x).toString());
                } else if (query.record().fieldName(x) == "volume_souple"){
                    volumeCuveSouple.append(query.value(x).toString());
                } else if (query.record().fieldName(x) == "date"){
                    date.append(query.value(x).toString());
                }
            }
        }
    }
    QList<QList<QString>> buffer;
    buffer.append(volumeCuveRigide);
    buffer.append(volumeCuveSouple);
    buffer.append(date);
    return buffer;
}

double bdd::lastTwoSouple(){ // Permets de savoir le débit d'entrée sortie de la cuve souple et donc pouvoir savoir le transfert entre les deux
    QList<QString> buffer;
    QSqlQuery query(*base);
    if(query.exec("SELECT volume_souple FROM fermeeconnect.eau ORDER BY id DESC LIMIT 2")){
        while(query.next()){
            for(int x=0; x<query.record().count(); x++){
               buffer.append(query.value(x).toString());
            }
        }
    }
    return buffer[1].toDouble() - buffer[0].toDouble();
}

bool bdd::detectFuite(){
    QList<QString> buffer;
    QSqlQuery query(*base);
    if(query.exec("SELECT conso_inst FROM fermeeconnect.eau WHERE date >= DATE(NOW()) - INTERVAL 30 MINUTE")){
        while(query.next()){
            for(int x=0; x<query.record().count(); x++){
               buffer.append(query.value(x).toString()); // ml/s
            }
        }
    }
    double somme = 0;
    for(int x = 0; x < buffer.length(); x++){
        somme += buffer[x].toDouble(); // mL.30min^-1
    }
    //qDebug() << somme << " / " << 300*1000;
    if (somme > 300*1000){
        return true;
    } else {
        return false;
    }
}
