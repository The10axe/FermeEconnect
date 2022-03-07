#ifndef IHMSETTINGS_H
#define IHMSETTINGS_H

#include <QMainWindow>

namespace Ui {
class IHMSettings;
}

class IHMSettings : public QMainWindow
{
    Q_OBJECT

public:
    explicit IHMSettings(QWidget *parent = nullptr, long diametreRigide = 1500, long longueurRigide = 2000, float volumeSoupleMax = 10000000, int pourcentMin = 20, int pourcentMax = 80); // Créer la classe IHMSettings avec des paramètres par défaut
    ~IHMSettings(); // Destructeurs

private slots:
    void on_diametre_valueChanged(int arg1); // Permets de mettre �  jour le calcule du volume de la cuve rigide

    void on_longueur_valueChanged(int arg1); // Permets de mettre �  jour le calcule du volume de la cuve rigide

    void on_VolumeSouple_valueChanged(double arg1); // Permets de mettre �  jour le calcule du volume de la cuve souple

    void on_saveAndQuit_clicked(); // Permet de sauvegarder les paramètres

    void on_Quit_clicked(); // Permet de quitter la fenêtre sans sauvegarder

    void on_Remplissage_min_valueChanged(int arg1);

    void on_Remplissage_max_valueChanged(int arg1);

public slots:
    void isLocked(bool); // Permet de géré l'attente d'accusé de réceptionj

private:
    Ui::IHMSettings *ui; //IHM

signals:
    void save(long, long, float, int, int); // Permet de mettre �  jour les paramètres
};

#endif // IHMSETTINGS_H
