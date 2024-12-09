#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <QObject>
#include <QNetworkAccessManager>

#include "camera.h"
#include "area.h"
#include "data.h"

#define HTTPCLIENT HttpClient::getInstance()
#define SERVER_URL "http://192.168.0.90:8080"

class HttpClient : public QObject
{
    Q_OBJECT
public:
    static HttpClient* getInstance();

    void getAllCamera();
    void getAllPeopleCountData(Camera& camera);
    void getPeopleCountDataByTime(Camera& camera, QString startTime, QString endTime);
    void getAllPeopleStayData(Camera& camera);
    void getPeopleStayDataByTime(Camera& camera, QString startTime, QString endTime);
    void getAllPeopleMoveData(Camera& camera);
    void getPeopleMoveDataByTime(Camera& camera, QString startTime, QString endTime);
    void insertArea(Camera& camera, Area& area);
    void deleteAreaAll(Camera& camera);
    void deleteArea(Camera& camera, Area& area);

signals:
    void allCameraFetched(QList<Camera> cameras);
    void allPeopleCountDataFetched(QList<PeopleCount> peopleCountData);
    void peopleCountDataByTimeFetched(QList<PeopleCount> peopleCountData);
    void allPeopleStayDataFetched(QList<PeopleStay> peopleStayData);
    void peopleStayDataByTimeFetched(QList<PeopleStay> peopleStayData);
    void allPeopleMoveDataFetched(QList<PeopleMove> peopleMoveData);
    void peopleMoveDataByTimeFetched(QList<PeopleMove> peopleMoveData);
    void areaInserted();
    void areaAllDeleted();
    void areaDeleted();

private slots:
    void onGetAllCameraFinished(QNetworkReply* reply);
    void onGetAllPeopleCountDataFinished(QNetworkReply* reply);
    void onGetPeopleCountDataByTimeFinished(QNetworkReply* reply);
    void onGetAllPeopleStayDataFinished(QNetworkReply* reply);
    void onGetPeopleStayDataByTimeFinished(QNetworkReply* reply);
    void onGetAllPeopleMoveDataFinished(QNetworkReply* reply);
    void onGetPeopleMoveDataByTimeFinished(QNetworkReply* reply);
    void onInsertAreaFinished(QNetworkReply* reply);
    void onDeleteAreaAllFinished(QNetworkReply* reply);
    void onDeleteAreaFinished(QNetworkReply* reply);

private:
    HttpClient(QObject *parent = nullptr);
    static HttpClient* instance;

    QJsonObject parseJson(QNetworkReply* reply);

    std::unique_ptr<QNetworkAccessManager> getAllCameraManager;
    std::unique_ptr<QNetworkAccessManager> getAllPeopleCountDataManager;
    std::unique_ptr<QNetworkAccessManager> getPeopleCountDataByTimeManager;
    std::unique_ptr<QNetworkAccessManager> getAllPeopleStayDataManager;
    std::unique_ptr<QNetworkAccessManager> getPeopleStayDataByTimeManager;
    std::unique_ptr<QNetworkAccessManager> getAllPeopleMoveDataManager;
    std::unique_ptr<QNetworkAccessManager> getPeopleMoveDataByTimeManager;
    std::unique_ptr<QNetworkAccessManager> insertAreaManager;
    std::unique_ptr<QNetworkAccessManager> deleteAreaAllManager;
    std::unique_ptr<QNetworkAccessManager> deleteAreaManager;
};

#endif // HTTPCLIENT_H