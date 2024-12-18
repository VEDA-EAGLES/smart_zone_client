#ifndef GRAPHDISPLAY_H
#define GRAPHDISPLAY_H

#include <QWidget>

#include "data.h"
#include "area.h"
#include "camera.h"
#include "sankeydiagram.h"

class QChart;

namespace Ui
{
    class GraphDisplay;
} // namespace UI

class GraphDisplay : public QWidget
{
    Q_OBJECT

public:
    GraphDisplay(QWidget* parent = nullptr);
    ~GraphDisplay();

private:
    Ui::GraphDisplay* ui;
    Camera camera;
    QMap<int, Area> areas;
    QList<PeopleCount> peopleCounts;
    QList<PeopleStay> peopleStays;
    QList<PeopleMove> peopleMoves;

    void init();
    void initConnect();

    void clearChart();
    void clearAreaButtons();
    QChart* createPeopleCountChart();
    QChart* createPeopleStayChart();
    SankeyDiagram* createPeopleMoveChart(int targetAreaId);
    void getPeopleCountData();
    void getPeopleStayData();
    void getPeopleMoveData();
    void getAreaData();

public slots:
    void setCamera(Camera& camera);
    void getData();
    void clear();
};

#endif // GRAPHDISPLAY_H