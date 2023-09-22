#ifndef MAP_H
#define MAP_H

#include <QWidget>

#include <QtCharts/QScatterSeries>
#include <QtCharts/QLegendMarker>
#include <QtCharts/QChartView>
#include <QtCharts/QAbstractAxis>
#include <QtCharts/QAbstractSeries>
#include <QtCharts/QChart>
#include <QtWidgets/QGraphicsView>
#include <QLineSeries>

namespace Ui {
class Map;
}
using namespace QtCharts;

class Map : public QWidget
{
    Q_OBJECT

public:
    explicit Map(QWidget *parent = nullptr);
    ~Map();
public slots:
    void drawCircle1(double R);
    void drawCircle2(double R);
    void drawCircle3(double R);
    void drawCurrentCoords(double x, double y);
    //void draw

protected:
    double x1,y1;
    double x2,y2;
    double x3,y3;
    Ui::Map *ui;
    QChartView *chartView = nullptr;
    QChart *chart = nullptr;
    QScatterSeries *beacon = nullptr;
    QScatterSeries *agentCoords = nullptr;
    QLineSeries *circle1 = nullptr;
    QLineSeries *circle2 = nullptr;
    QLineSeries *circle3 = nullptr;


};

#endif // MAP_H
