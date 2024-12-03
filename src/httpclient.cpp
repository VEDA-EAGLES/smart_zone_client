#include "httpclient.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QNetworkReply>

HttpClient* HttpClient::instance = nullptr;

HttpClient::HttpClient(QObject *parent)
    : QObject(parent)
{ 
    getAllCameraManager = std::make_unique<QNetworkAccessManager>();
    getAllPeopleCountDataManager = std::make_unique<QNetworkAccessManager>();
    getPeopleCountDataByTimeManager = std::make_unique<QNetworkAccessManager>();
    getAllPeopleMoveDataManager = std::make_unique<QNetworkAccessManager>();
    getPeopleMoveDataByTimeManager = std::make_unique<QNetworkAccessManager>();
    getAllPeopleStayDataManager = std::make_unique<QNetworkAccessManager>();
    getPeopleStayDataByTimeManager = std::make_unique<QNetworkAccessManager>();
    insertAreaManager = std::make_unique<QNetworkAccessManager>();
    deleteAreaAllManager = std::make_unique<QNetworkAccessManager>();
    deleteAreaManager = std::make_unique<QNetworkAccessManager>();

    connect(getAllCameraManager.get(), &QNetworkAccessManager::finished, this, &HttpClient::onGetAllCameraFinished);
    connect(getAllPeopleCountDataManager.get(), &QNetworkAccessManager::finished, this, &HttpClient::onGetAllPeopleCountDataFinished);
    connect(getPeopleCountDataByTimeManager.get(), &QNetworkAccessManager::finished, this, &HttpClient::onGetPeopleCountDataByTimeFinished);
    connect(getAllPeopleStayDataManager.get(), &QNetworkAccessManager::finished, this, &HttpClient::onGetAllPeopleStayDataFinished);
    connect(getPeopleStayDataByTimeManager.get(), &QNetworkAccessManager::finished, this, &HttpClient::onGetPeopleStayDataByTimeFinished);
    connect(getAllPeopleMoveDataManager.get(), &QNetworkAccessManager::finished, this, &HttpClient::onGetAllPeopleMoveDataFinished);
    connect(getPeopleMoveDataByTimeManager.get(), &QNetworkAccessManager::finished, this, &HttpClient::onGetPeopleMoveDataByTimeFinished);
    connect(insertAreaManager.get(), &QNetworkAccessManager::finished, this, &HttpClient::onInsertAreaFinished);
    connect(deleteAreaAllManager.get(), &QNetworkAccessManager::finished, this, &HttpClient::onDeleteAreaAllFinished);
    connect(deleteAreaManager.get(), &QNetworkAccessManager::finished, this, &HttpClient::onDeleteAreaFinished);
}

HttpClient* HttpClient::getInstance()
{
    if (instance == nullptr)
    {
        instance = new HttpClient();
    }
    return instance;
}

QJsonObject HttpClient::parseJson(QNetworkReply* reply)
{
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    return doc.object();
}

void HttpClient::getAllCamera()
{
    QNetworkRequest request;
    request.setUrl(QUrl(tr(SERVER_URL) + tr("/device/all")));
    getAllCameraManager->get(request);
}

void HttpClient::getAllPeopleCountData(Camera& camera)
{
    QNetworkRequest request;
    request.setUrl(QUrl(tr(SERVER_URL) + tr("/peoplecnt/all?camera_id=%1").arg(camera.id)));
    getAllPeopleCountDataManager->get(request);
}

void HttpClient::getPeopleCountDataByTime(Camera& camera, QString startTime, QString endTime)
{
    QNetworkRequest request;
    request.setUrl(QUrl(tr(SERVER_URL) + tr("/peoplecnt/unit?camera_id=%1").arg(camera.id) + "&start=" + startTime + "&end=" + endTime));
    getPeopleCountDataByTimeManager->get(request);
}

void HttpClient::getAllPeopleStayData(Camera& camera)
{
    QNetworkRequest request;
    request.setUrl(QUrl(tr(SERVER_URL) + tr("/peoplestay/all?camera_id=%1").arg(camera.id)));
    getAllPeopleStayDataManager->get(request);
}

void HttpClient::getPeopleStayDataByTime(Camera& camera, QString startTime, QString endTime)
{
    QNetworkRequest request;
    request.setUrl(QUrl(tr(SERVER_URL) + tr("/peoplestay/unit?camera_id=%1").arg(camera.id) + "&start=" + startTime + "&end=" + endTime));
    getPeopleStayDataByTimeManager->get(request);
}

void HttpClient::getAllPeopleMoveData(Camera& camera)
{
    QNetworkRequest request;
    request.setUrl(QUrl(tr(SERVER_URL) + tr("/peoplemove/all?camera_id=%1").arg(camera.id)));
    getAllPeopleMoveDataManager->get(request);
}

void HttpClient::getPeopleMoveDataByTime(Camera& camera, QString startTime, QString endTime)
{
    QNetworkRequest request;
    request.setUrl(QUrl(tr(SERVER_URL) + tr("/peoplemove/unit?camera_id=%1").arg(camera.id) + "&start=" + startTime + "&end=" + endTime));
    getPeopleMoveDataByTimeManager->get(request);
}

void HttpClient::insertArea(Camera& camera, Area& area)
{
    QNetworkRequest request;
    request.setUrl(QUrl(SERVER_URL "/area/insert"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject areaObject;
    areaObject["area_name"] = area.name;
    areaObject["camera_id"] = camera.id;
    areaObject["x"] = area.x;
    areaObject["y"] = area.y;
    areaObject["width"] = area.width;
    areaObject["height"] = area.height; 

    insertAreaManager.get()->post(request, QJsonDocument(areaObject).toJson());
}

void HttpClient::deleteAreaAll(Camera& camera)
{
    QNetworkRequest request;
    request.setUrl(QUrl(tr(SERVER_URL) + tr("/area/all?camera_id=%1").arg(camera.id)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    deleteAreaAllManager->deleteResource(request);
}

void HttpClient::deleteArea(Camera& camera, Area& area)
{
    QNetworkRequest request;
    request.setUrl(QUrl(tr(SERVER_URL) + tr("/area?camera_id=%1").arg(camera.id) + tr("&area_id=%1").arg(area.id)));
    deleteAreaManager->deleteResource(request);
}

void HttpClient::onGetAllCameraFinished(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        QJsonObject data = parseJson(reply);
        QJsonArray camerasArray = data["cameras"].toArray();

        QList<Camera> cameras;
        for (const QJsonValue& cameraValue : camerasArray)
        {
            QJsonObject cameraObject = cameraValue.toObject();
            Camera camera;
            camera.name = cameraObject["camera_name"].toString();
            camera.ip = cameraObject["camera_ip"].toString();
            camera.id = cameraObject["camera_id"].toInt();
            cameras.append(camera);
        }

        emit allCameraFetched(cameras);
    }
    reply->deleteLater();
}

void HttpClient::onGetAllPeopleCountDataFinished(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        QJsonObject data = parseJson(reply);
        QJsonArray peopleCountDataArray = data["data"].toArray();

        QList<PeopleCount> peopleCountData;
        for (const QJsonValue& peopleCountDataValue : peopleCountDataArray)
        {
            QJsonObject peopleCountDataObject = peopleCountDataValue.toObject();
            PeopleCount peopleCount;
            peopleCount.id = peopleCountDataObject["data_id"].toInt();
            peopleCount.areaId = peopleCountDataObject["area_id"].toInt();
            peopleCount.peopleCount = peopleCountDataObject["people_count"].toInt();
            peopleCount.startTime = peopleCountDataObject["start_time"].toInt();
            peopleCount.endTime = peopleCountDataObject["end_time"].toInt();
            peopleCountData.append(peopleCount);
        }

        emit allPeopleCountDataFetched(peopleCountData);
    }
    reply->deleteLater();
}

void HttpClient::onGetPeopleCountDataByTimeFinished(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        QJsonObject data = parseJson(reply);
        QJsonArray peopleCountDataArray = data["data"].toArray();

        QList<PeopleCount> peopleCountData;
        for (const QJsonValue& peopleCountDataValue : peopleCountDataArray)
        {
            QJsonObject peopleCountDataObject = peopleCountDataValue.toObject();
            PeopleCount peopleCount;
            peopleCount.id = peopleCountDataObject["data_id"].toInt();
            peopleCount.areaId = peopleCountDataObject["area_id"].toInt();
            peopleCount.peopleCount = peopleCountDataObject["people_count"].toInt();
            peopleCount.startTime = peopleCountDataObject["start_time"].toInt();
            peopleCount.endTime = peopleCountDataObject["end_time"].toInt();
            peopleCountData.append(peopleCount);
        }

        emit peopleCountDataByTimeFetched(peopleCountData);
    }
    reply->deleteLater();
}

void HttpClient::onGetAllPeopleStayDataFinished(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        QJsonObject data = parseJson(reply);
        QJsonArray peopleStayDataArray = data["data"].toArray();

        QList<PeopleStay> peopleStayData;
        for (const QJsonValue& peopleStayDataValue : peopleStayDataArray)
        {
            QJsonObject peopleStayDataObject = peopleStayDataValue.toObject();
            PeopleStay peopleStay;
            peopleStay.id = peopleStayDataObject["data_id"].toInt();
            peopleStay.areaId = peopleStayDataObject["area_id"].toInt();
            peopleStay.stayTime = peopleStayDataObject["stay_time"].toInt();
            peopleStay.startTime = peopleStayDataObject["start_time"].toInt();
            peopleStay.endTime = peopleStayDataObject["end_time"].toInt();
            peopleStayData.append(peopleStay);
        }

        emit allPeopleStayDataFetched(peopleStayData);
    }
    reply->deleteLater();
}

void HttpClient::onGetPeopleStayDataByTimeFinished(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        QJsonObject data = parseJson(reply);
        QJsonArray peopleStayDataArray = data["data"].toArray();

        QList<PeopleStay> peopleStayData;
        for (const QJsonValue& peopleStayDataValue : peopleStayDataArray)
        {
            QJsonObject peopleStayDataObject = peopleStayDataValue.toObject();
            PeopleStay peopleStay;
            peopleStay.id = peopleStayDataObject["data_id"].toInt();
            peopleStay.areaId = peopleStayDataObject["area_id"].toInt();
            peopleStay.stayTime = peopleStayDataObject["stay_time"].toInt();
            peopleStay.startTime = peopleStayDataObject["start_time"].toInt();
            peopleStay.endTime = peopleStayDataObject["end_time"].toInt();
            peopleStayData.append(peopleStay);
        }

        emit peopleStayDataByTimeFetched(peopleStayData);
    }
    reply->deleteLater();
}

void HttpClient::onGetAllPeopleMoveDataFinished(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        QJsonObject data = parseJson(reply);
        QJsonArray peopleMoveDataArray = data["data"].toArray();

        QList<PeopleMove> peopleMoveData;
        for (const QJsonValue& peopleMoveDataValue : peopleMoveDataArray)
        {
            QJsonObject peopleMoveDataObject = peopleMoveDataValue.toObject();
            PeopleMove peopleMove;
            peopleMove.id = peopleMoveDataObject["data_id"].toInt();
            peopleMove.fromAreaId = peopleMoveDataObject["from_area_id"].toInt();
            peopleMove.toAreaId = peopleMoveDataObject["to_area_id"].toInt();
            peopleMove.count = peopleMoveDataObject["count"].toInt();
            peopleMove.startTime = peopleMoveDataObject["start_time"].toInt();
            peopleMove.endTime = peopleMoveDataObject["end_time"].toInt();
            peopleMoveData.append(peopleMove);
        }

        emit allPeopleMoveDataFetched(peopleMoveData);
    }
    reply->deleteLater();
}

void HttpClient::onGetPeopleMoveDataByTimeFinished(QNetworkReply* reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        QJsonObject data = parseJson(reply);
        QJsonArray peopleMoveDataArray = data["data"].toArray();

        QList<PeopleMove> peopleMoveData;
        for (const QJsonValue& peopleMoveDataValue : peopleMoveDataArray)
        {
            QJsonObject peopleMoveDataObject = peopleMoveDataValue.toObject();
            PeopleMove peopleMove;
            peopleMove.id = peopleMoveDataObject["data_id"].toInt();
            peopleMove.fromAreaId = peopleMoveDataObject["from_area_id"].toInt();
            peopleMove.toAreaId = peopleMoveDataObject["to_area_id"].toInt();
            peopleMove.count = peopleMoveDataObject["count"].toInt();
            peopleMove.startTime = peopleMoveDataObject["start_time"].toInt();
            peopleMove.endTime = peopleMoveDataObject["end_time"].toInt();
            peopleMoveData.append(peopleMove);
        }

        emit peopleMoveDataByTimeFetched(peopleMoveData);
    }
    reply->deleteLater();
}

void HttpClient::onInsertAreaFinished(QNetworkReply* reply)
{
    emit areaInserted();
    reply->deleteLater();
}

void HttpClient::onDeleteAreaAllFinished(QNetworkReply* reply)
{
    emit areaAllDeleted();
    reply->deleteLater();
}

void HttpClient::onDeleteAreaFinished(QNetworkReply* reply)
{
    emit areaDeleted();
    reply->deleteLater();
}