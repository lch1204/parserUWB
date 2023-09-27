#include "map.h"
#include "ui_map.h"
#include "math.h"
#include <QDebug>
Map::Map(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Map)
{
    x3 = 130; y3 = 0;
    x2 = 0; y2 = 0;
    x1 = 0; y1 =60;
    ui->setupUi(this);
    beacon = new QScatterSeries();
    agentCoords = new QScatterSeries();
    circle1 = new QLineSeries();
    circle2 = new QLineSeries();
    circle3 = new QLineSeries();

    beacon->append(x1,y1);
    beacon->append(x2,y2);
    beacon->append(x3,y3);
    drawCircle1(45);
    drawCircle2(46);
    drawCircle3(100);

    chart = new QChart();
    chart->addSeries(beacon);
    chart->addSeries(circle1);
    chart->addSeries(circle2);
    chart->addSeries(circle3);
    chart->addSeries(agentCoords);
    chart->createDefaultAxes();
    chart->axes(Qt::Vertical).first()->setRange(-50,100);
    //chart->axes(Qt::Vertical).first()->setRange()
    chart->axes(Qt::Horizontal).first()->setRange(-50,150);
    chartView = new QChartView(chart);



    ui->verticalLayout_map->addWidget(chartView);



//    auto series0 = new QScatterSeries;
//    series0->setName("Агент - 1");
//    series0->setMarkerShape(QScatterSeries::MarkerShapeCircle);
//    series0->setMarkerSize(15.0);

//    series0->append(3, 8);


//    auto chart = new QChart;
//    chart->addSeries(series0);
//    chart->createDefaultAxes();
//    chart->setDropShadowEnabled(false);

//    chart->legend()->setMarkerShape(QLegend::MarkerShapeFromSeries);

//    QChartView *

//
}

Map::~Map()
{
    delete ui;
}

void Map::drawCircle1(double R)
{
    circle1->clear();
    for(int i=0; i<=360; i++){
        double alpha = i*M_PI/180;
        double x = x1+R*cos(alpha);
        double y = y1+R*sin(alpha);
        circle1->append(x,y);
    }
}

void Map::drawCircle2(double R)
{
    circle2->clear();

    for(int i=0; i<=360; i++){
        double alpha = i*M_PI/180;
        double x = x2+R*cos(alpha);
        double y = y2+R*sin(alpha);
        circle2->append(x,y);
    }

}

void Map::drawCircle3(double R)
{
    circle3->clear();

    for(int i=0; i<=360; i++){
        double alpha = i*M_PI/180;
        double x = x3+R*cos(alpha);
        double y = y3+R*sin(alpha);
        circle3->append(x,y);
    }
}

void Map::drawCurrentCoords(double x, double y)
{
    agentCoords->clear();
    agentCoords->append(x,y);

}

void Map::printInf(RecDataUWB msg)
{
//    ui->module0->setNum(static_cast<double>(msg.time01));
    ui->module1->setNum(X[41][0]);
    ui->module2->setNum(X[42][0]);
    ui->module3->setNum(X[43][0]);
    ui->xx->setNum(X[51][0]);
    ui->yy->setNum(X[52][0]);
    for (int i = 7; i>=0; i --)
    {
        if (msg.connection_field & (1<<i))
            prov[i] = 1;
//            qDebug() << "1";
        else
            prov[i] = 0;
//            qDebug() << "0";
    }
    ui->prov0->setNum(prov[0]);
    ui->prov1->setNum(prov[1]);
    ui->prov2->setNum(prov[2]);
    ui->prov3->setNum(prov[3]);
    ui->prov4->setNum(prov[4]);
    ui->prov5->setNum(prov[5]);
    ui->prov6->setNum(prov[6]);
    ui->prov7->setNum(prov[7]);
//    ui->error->setNum(static_cast<double>(msg.error_code));
    error_counter+=msg.error_code;
    ui->error->setNum(error_counter);
//    if (msg.error_code !=0)
//    {
//        error_counter++;
//    }
}
