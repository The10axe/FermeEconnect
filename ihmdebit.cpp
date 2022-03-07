#include "ihmdebit.h"
#include "ui_ihmdebit.h"

#include "ihmsettings.h"
#include "bdd.h"
#include "ihmhisto.h"

#include <QDateTime>

IHMDebit::IHMDebit(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::IHMDebit)
{
    ui->setupUi(this); // Création de l'IHM

    // Connection à la base de donnée
    base = new bdd();
    connect(base,SIGNAL(received(QString)),this,SLOT(update(QString)));

    // Initialisation des variables à vide
    diametreRigide = 0;
    longueurRigide = 0;
    maxVolumeRigide = 0;
    volumeSoupleMax = 0;
    pourcentMin = 0;
    pourcentMax = 0;

    base->getProperties(); // Obtention des paramètres pour les variables précédente

    setWindowIcon(QIcon(":/image/icon.PNG")); // Changement de l'icone
}

IHMDebit::~IHMDebit()
{
    delete ui; // On détruit l'IHM
}

void IHMDebit::update(QString data){
    QList<QString> valeur = data.split(";"); // On sépare les valeurs reçu
    if((valeur[0] != "get") && (valeur.size() == 4)){
        double volumeCuveRigide = valeur[0].toDouble(); // On récupère les valeurs
        double volumeCuveSouple = valeur[1].toDouble();
        double conso_inst = valeur[2].toDouble();
        valeur[3] = valeur[3].replace("T", " "); // On les convertie au bon format
        valeur[3].chop(4); // On retire les ms
        QDateTime last = QDateTime::fromString(valeur[3], "yyyy-MM-dd HH:mm:ss"); // On les convertie dans le bon format
        qint64 last_data = last.toSecsSinceEpoch(); // On prends le temps de la dernière données en s
        qint64 actual = QDateTime::currentDateTime().toSecsSinceEpoch(); // On prends le temps actuel en s
        if (actual-last_data > 60){ // Si ça fait plus d'une minute
            ui->connexion->setText("Attention!\nArduino potentiellement déconnecté"); // On avertie de la deconnexion
        } else {
            ui->connexion->setText("");
        }
        double buffer = base->lastTwoSouple(); // On regarde les changements dans la cuve souple
        if (buffer == 0){
            ui->Sens->setText(QString::fromUtf8("↑")); // Si ça ne bouge pas, la flèche pointe vers le haut
        } else if (buffer<0){
            ui->Sens->setText(QString::fromUtf8("←")); // Si elle se remplis, la flèche va de droite à gauche
        } else {
            ui->Sens->setText(QString::fromUtf8("→")); // Si elle se vide, la flèche va de gauche à droite
        }
        ui->Debit->setText("Transfert instantanée:\n"+QString::number(qAbs(buffer)/1000*60)+" L/min"); // On indique le débit en valeur absolue pour retirée le signe moins

        if(base->detectFuite()){
            ui->Consomation->setStyleSheet("color:red;");
        } else {
            ui->Consomation->setStyleSheet("color:black;");
        }
        // On mets à jour l'interface avec les données reçu
        ui->Cuve_1->setMaximum(maxVolumeRigide);
        ui->Cuve_1->setValue(volumeCuveRigide);
        ui->Cuve_2->setMaximum(maxVolumeRigide);
        ui->Cuve_2->setValue(volumeCuveRigide);
        ui->Cuve_S->setMaximum(volumeSoupleMax);
        ui->Cuve_S->setValue(volumeCuveSouple);
        ui->VCuve_1->setText("Volume : "+QString::number(volumeCuveRigide/1000)+" / "+QString::number(maxVolumeRigide/1000)+" L");
        ui->VCuve_2->setText("Volume : "+QString::number(volumeCuveRigide/1000)+" / "+QString::number(maxVolumeRigide/1000)+" L");
        ui->VCuve_S->setText("Volume : "+QString::number(volumeCuveSouple/1000)+" / "+QString::number(volumeSoupleMax/1000)+" L");
        ui->Consomation->setText("Conso. instantanée:\n"+QString::number(conso_inst/1000)+" L/s");
    } else if ((valeur[0] == "get") && (valeur.size() > 1)){ // On récupère le maximum si la trame commence par "get" (mot clé utilisé pour les paramètres de maximum)
        diametreRigide = valeur[1].toLong();
        longueurRigide = valeur[2].toLong();
        volumeSoupleMax = valeur[3].toFloat();
        pourcentMin = valeur[4].toInt();
        pourcentMax = valeur[5].toInt();
        maxVolumeRigide = (3.14159265359*((diametreRigide/2)*(diametreRigide/2)))*longueurRigide*0.001;
    }
}

void IHMDebit::getSettings(long diametre, long longueur, float volumeSouple, int pourcentMin, int pourcentMax){ // Permet d'envoyer les nouveau paramètre des cuves
    maxVolumeRigide = 3.14159265359*((diametre/2)*(diametre/2))*longueur*0.001;
    volumeSoupleMax = volumeSouple;
    diametreRigide = diametre;
    longueurRigide = longueur;
    base->updateProperties(diametre, longueur, volumeSouple, pourcentMin, pourcentMax);
}

void IHMDebit::on_settings_clicked()
{ // Permet d'ouvrir la fenêtre des paramètres
    IHMSettings *Settings = new IHMSettings(this, diametreRigide, longueurRigide, volumeSoupleMax, pourcentMin, pourcentMax); // Créer l'IHM avec les paramètres connues actuelle.
    connect(Settings,SIGNAL(save(long, long, float, int, int)),this,SLOT(getSettings(long, long, float, int, int))); // Lis la sauvegarde à la fonction de mise à jour
    Settings->show(); // Affiche la fenêtre
}

void IHMDebit::on_buttonHist_clicked()
{ // Permet d'ouvrir la fenêtre de l'historique
    ihmhisto *Histo = new ihmhisto(this, base);
    Histo->show();
}
