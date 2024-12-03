#ifndef DATA_H
#define DATA_H

struct PeopleCount
{
    int id;
    int areaId;
    int peopleCount;
    int startTime;
    int endTime;
};

struct PeopleStay
{
    int id;
    int areaId;
    int stayTime;
    int startTime;
    int endTime;
};

struct PeopleMove
{
    int id;
    int fromAreaId;
    int toAreaId;
    int count;
    int startTime;
    int endTime;
};

#endif // DATA_H