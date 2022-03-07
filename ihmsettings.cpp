#include "ihmsettings.h"
#include "ui_ihmsettings.h"

IHMSettings::IHMSettings(QWidget *parent, long diametreRigide, long longueurRigide, float volumeSoupleMax, int pourcentMin, int pourcentMax) :
    QMainWindow(parent),
    ui(new Ui::IHMSettings)
{
    // CrÃ©ation de l'IHM
    ui->setupUi(this);
    ui->diametre->setValue(diametreRigide);
    ui->longueur->setValue(longueurRigide);
    ui->VolumeSouple->setValue(volumeSoupleMax/1000000);
    ui->Remplissage_min->setValue(pourcentMin);
    ui->Remplissage_max->setValue(pourcentMax);
    setWindowIcon(QIcon(":/image/icon.PNG"));
}

IHMSettings::~IHMSettings()
{
    delete ui; // Destruction de l'IHM
}

void IHMSettings::on_diametre_valueChanged(int arg1)
{ // Permets de mettre Ã  jour le calcule du volume de la cuve rigide
    ui->estimationLitre->setText("Volume estimÃ©e: "+QString::number(3.14159265359*((arg1/2)*(arg1/2))*ui->longueur->value()*0.001/1000)+" L");
}

void IHMSettings::on_longueur_valueChanged(int arg1)
{ // Permets de mettre Ã  jour le calcule du volume de la cuve rigide
    ui->estimationLitre->setText("Volume estimÃ©e: "+QString::number(3.14159265359*((ui->diametre->value()/2)*(ui->diametre->value()/2))*arg1*0.001/1000)+" L");
}

void IHMSettings::on_VolumeSouple_valueChanged(double arg1)
{ // Permets de mettre Ã  jour le calcule du volume de la cuve souple
    ui->estimationLitreSouple->setText("Volume estimÃ©e: "+QString::number(arg1*1000)+" L");
}

void IHMSettings::on_saveAndQuit_clicked()
{ // Permet de sauvegarder les paramÃ¨tres
    emit save(ui->diametre->value(),ui->longueur->value(),ui->VolumeSouple->value()*1000000,ui->Remplissage_min->value(),ui->Remplissage_max->value());
    close();
}

void IHMSettings::on_Quit_clicked()
{ // Permet de quitter la fenÃªtre sans sauvegarder
    close();
}

void IHMSettings::isLocked(bool disabled){ // Permet de mettre Ã  jour les paramÃ¨tres
    disabled = !disabled;
    ui->saveAndQuit->setDisabled(disabled);
}


void IHMSettings::on_Remplissage_min_valueChanged(int arg1)
{
    if (ui->Remplissage_min->value() > ui->Remplissage_max->value()){
        int temp = ui->Remplissage_max->value();
        ui->Remplissage_max->setValue(ui->Remplissage_min->value());
        ui->Remplissage_min->setValue(temp);
    } else if (ui->Remplissage_min->value() == ui->Remplissage_max->value()) {
        ui->Remplissage_min->setValue(ui->Remplissage_min->value()-1);
    }
}

void IHMSettings::on_Remplissage_max_valueChanged(int arg1)
{
    if (ui->Remplissage_min->value() > ui->Remplissage_max->value()){
        int temp = ui->Remplissage_max->value();
        ui->Remplissage_max->setValue(ui->Remplissage_min->value());
        ui->Remplissage_min->setValue(temp);
    } else if (ui->Remplissage_min->value() == ui->Remplissage_max->value()) {
        ui->Remplissage_min->setValue(ui->Remplissage_min->value()-1);
    }
}
