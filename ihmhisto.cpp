#include "ihmhisto.h"

#include "bdd.h"
#include <QList>
#include <QDateTime>
#include <QtCharts>
using namespace QtCharts;
#include <QDateTimeAxis>

#include <QDebug>

ihmhisto::ihmhisto(QWidget *parent, bdd *getbase) : QMainWindow(parent)
{
    double mlMax = 0;
    QString plusGrandVol;
    QList<QList<QString>> data = getbase->getLastDays();
    QLineSeries *souple = new QLineSeries();
    QLineSeries *rigide = new QLineSeries();
    for(int x = 0; x < data[2].length(); x++){
        QString buffer = data[2][x].replace("T", " ");
        buffer.chop(4);
        QDateTime time = QDateTime::fromString(buffer, "yyyy-MM-dd HH:mm:ss");
        souple->append(time.toMSecsSinceEpoch(), data[1][x].toDouble()/1000);
        rigide->append(time.toMSecsSinceEpoch(), data[0][x].toDouble()*2/1000);
        if(mlMax < data[1][x].toDouble()){
            mlMax = data[1][x].toDouble();
            plusGrandVol = "souple";
        }
        if(mlMax < data[0][x].toDouble()*2){
            mlMax = data[0][x].toDouble()*2;
            plusGrandVol = "rigide";
        }
        /*qDebug() << buffer;
        qDebug() << time.toMSecsSinceEpoch();*/
    }
    souple->setName("Volume d'eau dans la cuve Souple");
    rigide->setName("Volume d'eau cumuler dans les cuves Rigide");

    QChart *chart = new QChart();
    chart->addSeries(souple);
    chart->addSeries(rigide);
    //chart->legend()->hide();
    chart->setTitle("Volume d'eau disponible dans les cuves");

    QDateTimeAxis *axisX = new QDateTimeAxis;
    axisX->setTickCount(7);
    axisX->setFormat("dd MMM - HH:mm:ss");
    axisX->setTitleText("Date");
    chart->addAxis(axisX, Qt::AlignBottom);
    souple->attachAxis(axisX);
    rigide->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis;
    axisY->setLabelFormat("%.2f");
    axisY->setTitleText("Volume d'eau (L)");
    chart->addAxis(axisY, Qt::AlignLeft);
    if (plusGrandVol == "rigide"){
        rigide->attachAxis(axisY);
        souple->attachAxis(axisY);
    } else {
        souple->attachAxis(axisY);
        rigide->attachAxis(axisY);
    }


    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    setWindowTitle("Historique");
    setCentralWidget(chartView);
    resize(820, 600);
    setMaximumSize(820,600);
    setMinimumSize(820,600);
    setWindowIcon(QIcon(":/image/icon.PNG"));
    show();
}
