#include "ui_graphdisplay.h"
#include "graphdisplay.h"
#include "httpclient.h"

#include <QChartView>
#include <QChart>
#include <QLineSeries>
#include <QBoxPlotSeries>
#include <QValueAxis>
#include <QCategoryAxis>
#include <QPushButton>

#include <QRandomGenerator>

GraphDisplay::GraphDisplay(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::GraphDisplay)
{
    init();
}

GraphDisplay::~GraphDisplay()
{
    delete ui;
}

void GraphDisplay::init()
{
    ui->setupUi(this);
    initConnect();
    ui->durationWidget->hide();
}

void GraphDisplay::initConnect()
{
    connect(HTTPCLIENT, &HttpClient::allPeopleCountDataFetched, this, [=](QList<PeopleCount> peopleCountData) {
        peopleCounts = peopleCountData;
    });
    connect(HTTPCLIENT, &HttpClient::allPeopleStayDataFetched, this, [=](QList<PeopleStay> peopleStayData) {
        peopleStays = peopleStayData;
    });
    connect(HTTPCLIENT, &HttpClient::allPeopleMoveDataFetched, this, [=](QList<PeopleMove> peopleMoveData) {
        peopleMoves = peopleMoveData;
    });
    connect(HTTPCLIENT, &HttpClient::allAreaByCameraFetched, this, [=](QList<Area> areaData) {
        areas.clear();
        for (const auto& area : areaData) {
            areas[area.id] = area;
        }
    });

    connect(ui->peoplecountButton, &QPushButton::clicked, this, [=]() {
        if (camera.name.isEmpty()) {
            ui->peoplecountButton->setChecked(false);
            return;
        }
        clearChart();
        clearAreaButtons();
        ui->peoplecountButton->setChecked(true);
        ui->peoplestayButton->setChecked(false);
        ui->peoplemoveButton->setChecked(false);
        QChartView* chartView = new QChartView(this);
        chartView->setRenderHint(QPainter::Antialiasing);
        ui->chartWidget->layout()->addWidget(chartView);
        chartView->setChart(createPeopleCountChart());
    });

    connect(ui->peoplestayButton, &QPushButton::clicked, this, [=]() {
        if (camera.name.isEmpty()) {
            ui->peoplestayButton->setChecked(false);
            return;
        }
        clearChart();
        clearAreaButtons();
        ui->peoplestayButton->setChecked(true);
        ui->peoplecountButton->setChecked(false);
        ui->peoplemoveButton->setChecked(false);
        QChartView* chartView = new QChartView(this);
        ui->chartWidget->layout()->addWidget(chartView);
        chartView->setChart(createPeopleStayChart());
    });

    connect(ui->peoplemoveButton, &QPushButton::clicked, this, [=]() {
        if (camera.name.isEmpty()) {
            ui->peoplemoveButton->setChecked(false);
            return;
        }
        clearChart();
        clearAreaButtons();
        for (auto& area : areas) {
            QPushButton* button = new QPushButton(area.name);
            connect(button, &QPushButton::clicked, this, [=]() {
                clearChart();
                ui->chartWidget->layout()->addWidget(createPeopleMoveChart(area.id));
            });
            ui->areaButtonLayout->addWidget(button);
        }
        ui->peoplemoveButton->setChecked(true);
        ui->peoplecountButton->setChecked(false);
        ui->peoplestayButton->setChecked(false);
    });
}

void GraphDisplay::clearChart()
{
    QLayoutItem* item;
    while ((item = ui->chartWidget->layout()->takeAt(0)) != nullptr) {
        item->widget()->hide();
        ui->chartWidget->layout()->removeItem(item);
    }
    update();
}

void GraphDisplay::clearAreaButtons()
{
    QLayoutItem* item;
    while ((item = ui->areaButtonLayout->takeAt(0)) != nullptr) {
        item->widget()->hide();
        ui->areaButtonLayout->removeItem(item);
    }
    update();
}

QChart* GraphDisplay::createPeopleCountChart()
{
    // dummy data
    QList<PeopleCount> dataList;
    qint64 startTime = QDateTime::currentSecsSinceEpoch(); // 현재 시간 기준 시작 시간
    int areaCount = 5;
    int dataCountPerArea = 100;
    for (int areaId = 1; areaId <= areaCount; ++areaId) {
        for (int i = 0; i < dataCountPerArea; ++i) {
            PeopleCount entry;
            entry.id = (areaId - 1) * dataCountPerArea + i + 1; // 고유 ID
            entry.areaId = areaId; // 영역 ID
            entry.peopleCount = QRandomGenerator::global()->bounded(0, 100); // 0~99 랜덤 인원 수
            
            // 시간 설정
            entry.startTime = startTime + i * 60;
            entry.endTime = entry.startTime + 60;
            
            dataList.append(entry);
        }
    }
    peopleCounts = dataList;
    // -------------------------------------------------

    QChart* chart = new QChart();
    chart->setTitle("영역별 시간 - 인원 수");
    
    QCategoryAxis* axisX = new QCategoryAxis();
    QValueAxis* axisY = new QValueAxis();
    
    axisX->setTitleText("시간");
    axisX->setLabelsAngle(-90);
    axisY->setLabelFormat("%d");
    axisY->setTitleText("인원 수");


    QMap<int, QLineSeries*> areaSeriesMap;
    QMap<int, QList<PeopleCount>> areaPeopleCounts;

    for (const auto& peoplecount : peopleCounts) {
        if (!areaPeopleCounts.contains(peoplecount.areaId)) {
            areaPeopleCounts[peoplecount.areaId] = QList<PeopleCount>();
        }
        areaPeopleCounts[peoplecount.areaId].append(peoplecount);
    }

    qint64 minTime = LLONG_MAX;
    qint64 maxTime = LLONG_MIN;
    int maxPeopleCount = 0;
    int minPeopleCount = INT_MAX;
    
    for (auto& areaPeopleCnts : areaPeopleCounts) {
        int areaId = areaPeopleCnts[0].areaId;
        if (!areaSeriesMap.contains(areaId)) {
            QLineSeries* series = new QLineSeries();
            series->setName(tr("%1").arg(areas[areaId].name));
            areaSeriesMap[areaId] = series;
        }

        for (const auto& peoplecount : areaPeopleCnts) {
            minTime = qMin(minTime, peoplecount.endTime);
            maxTime = qMax(maxTime, peoplecount.endTime);
            minPeopleCount = qMin(minPeopleCount, peoplecount.peopleCount);
            maxPeopleCount = qMax(maxPeopleCount, peoplecount.peopleCount);
        }
    }

    int numSegments = 30;
    qint64 interval = (maxTime - minTime) / numSegments;
    for (auto& areaPeopleCnts : areaPeopleCounts) {
        int areaId = areaPeopleCnts[0].areaId;
        for (int segment = 0; segment < numSegments; ++segment) {
            qint64 segmentStartTime = minTime + segment * interval;
            qint64 segmentEndTime = segmentStartTime + interval;

            int peopleCountSum = 0;
            int count = 0;
            for (const auto& peoplecount : areaPeopleCnts) {
                if (peoplecount.endTime >= segmentStartTime && peoplecount.endTime < segmentEndTime) {
                    peopleCountSum += peoplecount.peopleCount;
                    count++;
                }
            }

            if (count > 0) {
                QLineSeries* series = areaSeriesMap[areaId];
                series->append(segmentStartTime, peopleCountSum / count);
            }
        }

    }

    int tickCount = 10;
    qint64 tickInterval = (maxTime - minTime) / tickCount;

    for (qint64 time = minTime; time <= maxTime; time += tickInterval) {
        QString label = QDateTime::fromSecsSinceEpoch(time).toString("yy/MM/dd  hh:mm");
        axisX->append(label, time);
    }
    qint64 paddingTime = (maxTime - minTime) * 0.05;
    int paddingPeopleCount = (maxPeopleCount - minPeopleCount) * 0.1;
    axisX->setMin(minTime - paddingTime);
    axisX->setMax(maxTime + paddingTime);
    axisY->setMin(minPeopleCount - paddingPeopleCount);
    axisY->setMax(maxPeopleCount + paddingPeopleCount);

    for (auto* series : areaSeriesMap) {
        chart->addSeries(series);
        chart->setAxisX(axisX, series);
        chart->setAxisY(axisY, series);
    }

    return chart;
}

QChart* GraphDisplay::createPeopleStayChart()
{
    // 데이터가 많아지면 boxset 분포가 찌부되는 현상 해결 필요
    // dummy data
    QList<PeopleStay> dataList;
    int areaCount = 5;
    int dataCountPerArea = 20;
    for (int areaId = 1; areaId <= areaCount; ++areaId) {
        for (int i = 0; i < dataCountPerArea; ++i) {
            PeopleStay entry;
            entry.id = (areaId - 1) * dataCountPerArea + i + 1; // 고유 ID
            entry.areaId = areaId; // 영역 ID
            entry.stayTime = QRandomGenerator::global()->bounded(3, 100); // 0~99 랜덤 체류 시간
            
            // 시간 설정
            entry.startTime = QDateTime::currentSecsSinceEpoch() + i * 60;
            entry.endTime = entry.startTime + 60;
            
            dataList.append(entry);
        }
    }
    peopleStays = dataList;

    // ==================================================
    QChart* chart = new QChart();
    chart->setTitle("영역 - 체류 시간");

    QBoxPlotSeries* series = new QBoxPlotSeries();

    QMap<int, QList<int>> areaStayTimes;
    for (auto& peopleStay : peopleStays) {
        if (!areaStayTimes.contains(peopleStay.areaId)) {
            areaStayTimes[peopleStay.areaId] = QList<int>();
        }
        areaStayTimes[peopleStay.areaId].append(peopleStay.stayTime);
    }

    int minStay = INT_MAX;
    int maxStay = 0;

    for (auto& areaId : areaStayTimes.keys()) {
        QList<int>& areaStayTime = areaStayTimes[areaId];
        std::sort(areaStayTime.begin(), areaStayTime.end());
        
        int size = areaStayTime.size();
        int minStayTime = areaStayTime.first();
        int lowerQuartileStayTime = areaStayTime[size / 4];
        int medianStayTime = (size % 2)? areaStayTime[size / 2] : (areaStayTime[size / 2 - 1] + areaStayTime[size / 2]) / 2;
        int upperQuartileStayTime = areaStayTime[size * 3 / 4];
        int maxStayTime = areaStayTime.last();


        QBoxSet* boxSet = new QBoxSet();
        boxSet->setLabel(tr("%1").arg(areas[areaId].name));
        boxSet->setValue(QBoxSet::LowerExtreme, minStayTime);
        boxSet->setValue(QBoxSet::LowerQuartile, lowerQuartileStayTime);
        boxSet->setValue(QBoxSet::Median, medianStayTime);
        boxSet->setValue(QBoxSet::UpperQuartile, upperQuartileStayTime);
        boxSet->setValue(QBoxSet::UpperExtreme, maxStayTime);
        series->append(boxSet);

        minStay = qMin(minStay, minStayTime);
        maxStay = qMax(maxStay, maxStayTime);
    }
    qDebug() << "minStay" << minStay << "maxStay" << maxStay;
    int paddingStayTime = (maxStay - minStay) * 0.05;

    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->axisX()->setTitleText("영역");
    chart->axisY()->setMax(maxStay + paddingStayTime);
    chart->axisY()->setMin(minStay - paddingStayTime);
    chart->axisY()->setTitleText("체류 시간(초)");
    chart->legend()->setVisible(false);

    return chart;
}

SankeyDiagram* GraphDisplay::createPeopleMoveChart(int targetAreaId)
{
    SankeyDiagram* sankeyDiagram = new SankeyDiagram();

    // dummy data
    sankeyDiagram->addNode("Source A", 40.0, QColor(200, 100, 100), 0);
    sankeyDiagram->addNode("Source B", 20.0, QColor(100, 200, 100), 0);
    sankeyDiagram->addNode("Target 1", 25.0, QColor(100, 100, 200), 1);
    sankeyDiagram->addNode("Target 2", 20.0, QColor(200, 200, 100), 1);
    sankeyDiagram->addNode("Target 3", 25.0, QColor(200, 100, 200), 1);
    sankeyDiagram->addNode("qwer", 60, QColor(100,100,100), 2);
    sankeyDiagram->addNode("지훈", 50, QColor(200, 100, 50), 2);

    sankeyDiagram->addLink("Source A", "Target 1", 20.0);
    sankeyDiagram->addLink("Source A", "Target 2", 10.0);
    sankeyDiagram->addLink("Source B", "Target 2", 15.0);
    sankeyDiagram->addLink("Source B", "Target 3", 15.0);
    sankeyDiagram->addLink("Source A", "Target 3", 5.0);
    sankeyDiagram->addLink("Target 3", "qwer", 25.0);
    sankeyDiagram->addLink("Target 2", "지훈", 10);
    sankeyDiagram->addLink("Target 1", "지훈", 40);
    
    sankeyDiagram->drawDiagram();
    // -------------------------

    QList<PeopleMove> targetPeopleMoves;
    for (auto& peopleMove : peopleMoves) {
        if (peopleMove.toAreaId == targetAreaId || peopleMove.fromAreaId == targetAreaId) {
            targetPeopleMoves.append(peopleMove);
        }
    }
    
    QMap<int, int> valueFromAreaToTarget, valueFromTargetToArea;
    int targetAreaValue = 0;
    for (auto& peopleMove : targetPeopleMoves) {
        targetAreaValue += peopleMove.count;
        if (peopleMove.toAreaId == targetAreaId) {
            valueFromAreaToTarget[peopleMove.fromAreaId] += peopleMove.count;
        } else {
            valueFromTargetToArea[peopleMove.toAreaId] += peopleMove.count;
        }
    }
    targetAreaValue /= 2;
    
    // add nodes
    for (auto& fromArea : valueFromAreaToTarget.keys()) {
        sankeyDiagram->addNode(tr("from %1").arg(areas[fromArea].name), valueFromAreaToTarget[fromArea], areas[fromArea].color, 0);
    }
    sankeyDiagram->addNode(tr("%1").arg(areas[targetAreaId].name), targetAreaValue, areas[targetAreaId].color, 1);
    for (auto& toArea : valueFromTargetToArea.keys()) {
        sankeyDiagram->addNode(tr("to %1").arg(areas[toArea].name), valueFromTargetToArea[toArea], areas[toArea].color, 2);
    }

    // add links
    for (auto& fromArea : valueFromAreaToTarget.keys()) {
        sankeyDiagram->addLink(tr("from %1").arg(areas[fromArea].name), tr("%1").arg(areas[targetAreaId].name), valueFromAreaToTarget[fromArea]);
    }
    for (auto& toArea : valueFromTargetToArea.keys()) {
        sankeyDiagram->addLink(tr("%1").arg(areas[targetAreaId].name), tr("to %1").arg(areas[toArea].name), valueFromTargetToArea[toArea]);
    }

    sankeyDiagram->drawDiagram();
    
    return sankeyDiagram;
}

// slots
void GraphDisplay::setCamera(Camera& camera)
{
    if (this->camera.name == camera.name) {
        qDebug() << "same camera setted";
        return;
    }
    qDebug() << "new camera set" << camera.name;
    this->camera = camera;
    ui->peoplecountButton->setChecked(false);
    ui->peoplestayButton->setChecked(false);
    ui->peoplemoveButton->setChecked(false);
    clearChart();
    getData();
}

void GraphDisplay::getData()
{
    if (camera.name.isEmpty()) {
        qDebug() << "camera is not set";
        return;
    }
    HTTPCLIENT->getAllPeopleCountData(camera);
    HTTPCLIENT->getAllPeopleStayData(camera);
    HTTPCLIENT->getAllPeopleMoveData(camera);
    HTTPCLIENT->getAllAreaByCamera(camera);
}