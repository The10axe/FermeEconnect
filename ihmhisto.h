#ifndef IHMHISTO_H
#define IHMHISTO_H

#include <QMainWindow>

class bdd;

class ihmhisto : public QMainWindow
{
    Q_OBJECT
public:
    explicit ihmhisto(QWidget *parent = nullptr, bdd *getbase = nullptr);

signals:

};

#endif // IHMHISTO_H
