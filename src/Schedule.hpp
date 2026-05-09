#ifndef __SCHEDULE_HPP__
#define __SCHEDULE_HPP__

#pragma once
#pragma pack(push, 1)
typedef struct {
    int triangle;
    int avgHeight;
    int waterHeight;
} PhysicsTriangle;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    PhysicsTriangle triangle;
    double delay;
} ScheduledTriangle;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    ScheduledTriangle *triangles;
    int count;
    volatile bool * capacity;
} ScheduleContext;
#pragma pack(pop)

#endif