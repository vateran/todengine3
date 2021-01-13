#pragma once
#include <chrono>
#include <thread>
#include "tod/object.h"
namespace tod
{

class TimeMgr : public SingletonDerive<TimeMgr, Object>
{
public:
    using clock = std::chrono::high_resolution_clock;
    using time_unit = std::chrono::microseconds;
    typedef clock::time_point TimePoint;

public:
    TimeMgr()
    {
        this->systemStart = clock::now();
        this->prevNow = this->systemStart;
    }
    
    void update() noexcept
    {
        auto cur = clock::now();
        
        auto now = (cur - this->systemStart);
        this->nowT = std::chrono::duration_cast<time_unit>
            (now).count() / 1000000.0f;
        
        auto delta = std::chrono::duration_cast<time_unit>
            (cur - this->prevNow) / 1000000.0f;
        this->deltaT = delta.count();
        this->prevNow = cur;
    }

    void sleep(int32 duration_msec) const noexcept
    {
        std::this_thread::sleep_for(time_unit(duration_msec));
    }
    
    inline double now() const noexcept { return this->nowT; }
    inline double delta() const noexcept { return this->deltaT; }

    static inline uint64 cpuCounter()
    {
#if defined (TOD_PLATFORM_WINDOWS)
        return __rdtsc();
#else
        TOD_ASSERT(false, "no implementation");
        return 0;
#endif
    }

private:
    TimePoint systemStart;
    TimePoint prevNow;
    double nowT;
	double deltaT;
    
};

}
