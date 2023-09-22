#include "map.h"
#include "ui_map.h"
#include "math.h"
#include <QDebug>
Map::Map(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Map)
{
    x3 = 435; y3 = 0;
    x2 = 0; y2 = 0;
    x1 = 0; y1 =245;
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
    chart->axes(Qt::Vertical).first()->setRange(-50,300);
    //chart->axes(Qt::Vertical).first()->setRange()
    chart->axes(Qt::Horizontal).first()->setRange(-50,500);
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
