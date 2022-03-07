#ifndef IHMDEBIT_H
#define IHMDEBIT_H

#include <QMainWindow>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class IHMDebit; }
QT_END_NAMESPACE

class IHMSettings; // Classe de la fenêtre des paramètres vide

class bdd;

class IHMDebit : public QMainWindow
{
    Q_OBJECT // Possède des signals/slots
public:
    IHMDebit(QWidget *parent = nullptr); // Constructeur
    ~IHMDebit(); // Destructeur

private:
    bdd *base;
    Ui::IHMDebit *ui; // IHM
    float maxVolumeRigide; // Contenue du volume max de la cuve rigide
    float volumeSoupleMax; // Contenue du volume max de la cuve souple
    long diametreRigide; // Diamètre de la cuve rigide
    long longueurRigide; // Longueur de la cuve rigide
    int pourcentMin;
    int pourcentMax;

public slots:
    void update(QString); // Slot nécessaire pour la classe Arduino, permet de mettre �  jour l'interface

signals:
    void send(QString, bool); // Permet d'envoyer un message avec ou sans accusée de réception

private slots:
    void on_settings_clicked(); // Permet d'ouvrir les paramètres
    void getSettings(long, long, float, int, int); // Permet d'obtenir les paramètres pour les mettres �  jours
    void on_buttonHist_clicked();
};
#endif // IHMDEBIT_H
