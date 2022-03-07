#ifndef HOME_H
#define HOME_H
#include <QLabel>
#include <QWidget>
#include <QMainWindow>
#include <QDebug>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QUrl>

QT_BEGIN_NAMESPACE
namespace Ui { class home; }
QT_END_NAMESPACE

class home : public QMainWindow
{
    Q_OBJECT

public:
    home(QWidget *parent = nullptr);
    ~home();

private:
    int etat;
    QPushButton *eau;
    QPushButton *camera;
    QPushButton *fromagerie;
    QPushButton *champs;
    QVBoxLayout *layout;
    QHBoxLayout *layoutH;
    QHBoxLayout *layoutH2;
    QLabel *image;
    QPushButton *statistique;

private slots:
    //void on_comboBox_currentIndexChanged(const QString &arg1); // Temporaire
    void on_eau(); // Permet d'ouvrir l'IHM de gestion des cuves
    void on_camera(); // Permet d'ouvrir l'IHM de gestion des cameras
    void on_fromagerie(); // Permet d'ouvrir l'IHM de la froragerie
    void on_champs(); // Permer d'ouvir l'IHM de la gestion du marechage
    void on_statistique(); // permet d'ouvrir l'IHM de la gestion des statistique

};
#endif // HOME_H
