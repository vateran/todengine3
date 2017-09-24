#pragma once
#include <chrono>
#include "tod/object.h"
namespace tod
{

class TimeMgr : public SingletonDerive<TimeMgr, Object>
{
public:
    using clock = std::chrono::system_clock;
    using time_unit = std::chrono::milliseconds;
    typedef clock::time_point TimePoint;

public:
    TimeMgr()
    {
        this->systemStart = clock::now();
        this->prevNow = this->systemStart;
    }
    
    void update()
    {
        auto cur = clock::now();
        
        auto now = (cur - this->systemStart);
        this->nowT = std::chrono::duration_cast<time_unit>
            (now).count() / 1000.0f;
        
        auto delta = std::chrono::duration_cast<time_unit>
            (cur - this->prevNow) / 1000.0f;
        this->deltaT = delta.count();
        this->prevNow = cur;
    }
    
    double now()
    {
        return this->nowT;
    }
    float delta() const
    {
        return deltaT;
    }

private:
    TimePoint systemStart;
    TimePoint prevNow;
    double nowT;
    float deltaT;
    
};

}
