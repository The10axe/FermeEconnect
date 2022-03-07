#include "home.h"
#include <QDebug>
#include <QPushButton>
#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QUrl>
#include <QDesktopServices>
#include <QFileDialog>
#include <QLabel>
#include <QWidget>
#include <QUrl>
#include "ihmdebit.h"
#include "camera.h"
#include "fromagerie.h"
#include "arrosage.h"
#include "stati.h"

home::home(QWidget *parent)
    : QMainWindow(parent)

{

//widget



    eau = new QPushButton (this); // eau
    eau->setGeometry(50,250,160,160);
    eau->setIcon(QIcon(":/pics/IMG/cuve.png"));
    eau->setIconSize(QSize(150,150));

    camera = new QPushButton (this); //camera
    camera->setGeometry(250,50,160,160);
    camera->setIcon(QIcon(":/pics/IMG/camera.png"));
    camera->setIconSize(QSize(150,150));

    fromagerie = new QPushButton(this); //fromagerie
    fromagerie->setGeometry(50,50,160,160);
    fromagerie->setIcon(QIcon(":/pics/IMG/fromagerie.png"));
    fromagerie->setIconSize(QSize(150,150));

    champs = new QPushButton(this); //maraichage
    champs->setGeometry(250,250,160,160);
    champs->setIcon(QIcon(":/pics/IMG/champs.png"));
    champs->setIconSize(QSize(150,150));

    image = new QLabel(this);  //logo
    image->setPixmap(QPixmap(":/pics/IMG/logo.PNG"));
    //image->setFixedSize(QSize(595,274));
    image->setGeometry(10,425,440,203);
    image->setScaledContents(true);
    setFixedSize(460,640);

    statistique = new QPushButton (this);//
    statistique->setGeometry(210,210,40,40);
    statistique->setIcon(QIcon(":/pics/IMG/stat.png"));
    statistique->setIconSize(QSize(30,30));

    // ajout des widget a la disposition

     setWindowIcon(QIcon(":/image/icon.PNG"));
     connect(eau,SIGNAL(clicked()),this,SLOT(on_eau()));
     connect(camera,SIGNAL(clicked()),this,SLOT(on_camera()));
     connect(fromagerie,SIGNAL(clicked()),this,SLOT(on_fromagerie()));
     connect(champs,SIGNAL(clicked()),this,SLOT(on_champs()));
     connect(statistique,SIGNAL(clicked()),this,SLOT(on_statistique()));
    setWindowTitle("Ferme ECOnnect Accueil");
}

home::~home()
{

}

void home::on_eau(){
    IHMDebit *fenetre = new IHMDebit();
    fenetre->show();
}

void home::on_camera(){
    Camera *fenetre = new Camera();
    fenetre->show();
}
void home::on_fromagerie(){
    Fromagerie *fenetre = new Fromagerie();
    fenetre->show();
}
void home::on_statistique(){
    stati *fenetre = new stati();
    fenetre->show();
}

void home::on_champs(){
    Arrosage *fenetre = new Arrosage();
    fenetre->show();
}


